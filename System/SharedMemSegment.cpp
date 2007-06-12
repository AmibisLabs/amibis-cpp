#include <System/SharedMemSegment.h>

#include <System/Thread.h>

using namespace Omiscid;

SharedMemSegment::SharedMemSegment()
{
    SegmentKey    = 0;
    SegAddress    = NULL;
    AccessFlags = NULL;
    UserAddress    = NULL;

#ifdef WIN32
    Name.Empty();
    hSegMemFile = NULL;
#else
    Name    = (key_t)0;
    iSegMem = -1;
#endif
}

SharedMemSegment::~SharedMemSegment()
{
    Close();
}

#ifndef WIN32 // for linux and OS X
bool SharedMemSegment::RemoveSharedMemSegment( unsigned int SegtKey )
{
    struct shmid_ds SharedMemInfos;
    int iSharedMem;

    // Check if we get information about it
    iSharedMem = shmget( SegtKey, 0, 0 );
    if ( iSharedMem == -1 || shmctl( iSharedMem, IPC_STAT, &SharedMemInfos ) == -1 )
    {
        return false;
    }

    // Is it freeable ?
    if ( SharedMemInfos.shm_nattch == 0 )
    {
        // Try to remove it
        return (shmctl( iSharedMem, IPC_RMID, (struct shmid_ds*)NULL ) != -1 );
    }

    return false;
}
#endif

void SharedMemSegment::SetName( unsigned int SegKey )
{
    TemporaryMemoryBuffer Tmpc(32);

    // Store key
    SegmentKey = SegKey;

#ifdef WIN32
    // Create a Name for this segment
    Name = "Global\\OMiSCID_";
    sprintf( Tmpc, "%8.8x", SegKey );
    Name += Tmpc;
#else
    Name = (key_t)SegKey;
#endif
}

void SharedMemSegment::InitNewSharedMemSegment( bool GrantWriteAccess )
{
    AccessFlags = (SharedMemSegmentFlags*)SegAddress;
    UserAddress = ((char*)SegAddress)+GetSizeOfReservedSpace();

    // Is it possible to write in it ?
    if ( GrantWriteAccess == false )
    {
        // If not, say it !
        *AccessFlags = (SharedMemSegmentFlags)(*AccessFlags | SharedMemSegment_ReadOnly);
    }

    // Ok, the memory segment is initialized
    *AccessFlags = (SharedMemSegmentFlags)(*AccessFlags | SharedMemSegment_Initialized);
}

bool SharedMemSegment::GetSharedMemSegmentAccess( bool WantWriteAccess )
{
    AccessFlags = (SharedMemSegmentFlags*)SegAddress;
    UserAddress = ((char*)SegAddress)+GetSizeOfReservedSpace();

    if ( WantWriteAccess == true )
    {
        // We wait for the SegMem to be initialized
        // No need to use a mutex as there is only one producer and many consumers
        int NbTry;
        for( NbTry = 0; NbTry < 10; NbTry++ )
        {
            if ( ((*AccessFlags) & SharedMemSegment_Initialized ) )
            {
                break;
            }
            Thread::Sleep(10);
        }
        if ( NbTry == 10 )
        {
            OmiscidTrace( "Could not open segment %8.8x (too long to initialize).\n", SegmentKey );
            Close();
            return false;
        }

        if ( ((*AccessFlags) & SharedMemSegment_ReadOnly) )
        {
            OmiscidTrace( "Could not open segment %8.8x with write access.\n", SegmentKey );
            Close();
            return false;
        }
    }

    return true;
}

bool SharedMemSegment::CreateAndOpen( unsigned int SegKey, int SegmentSize, bool GrantWriteAccessToOthers /* = false */ )
{
    int TotalSize;

    if ( SegKey == 0 || SegmentSize == 0 )
    {
        throw SimpleException("SharedMemSegment::CreateAndOpen bad parameter(s)", 0);
    }

    // Set name of this segment
    SetName( SegKey );

    // Compute TotalSize of the segment (including all data).
    TotalSize = SegmentSize + GetSizeOfReservedSpace();

#ifdef WIN32
    // Create a map file call Name
    hSegMemFile = CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,(DWORD)TotalSize,Name.GetStr());
    if ( hSegMemFile == 0 )
    {
        OmiscidTrace( "Could not open segment %8.8x\n", SegKey );
        return false;
    }
    // Get adress of the shared memory segment
    SegAddress = (void*)MapViewOfFile( hSegMemFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
    if ( SegAddress == NULL )
    {
        OmiscidTrace( "Could not open segment %8.8x (no access)\n", SegKey );
        Close();
        return false;
    }
#else
    OmiscidTrace( "%d\n", Name );
    // Create new segment or fail
    iSegMem = shmget( Name, TotalSize, IPC_CREAT | IPC_EXCL | 0666 );
    if ( iSegMem == -1 )
    {
        // Try to remove it
        if ( RemoveSharedMemSegment( Name ) == false || (iSegMem = shmget( Name, TotalSize, IPC_CREAT | IPC_EXCL | 0666 )) == -1 )
        {
            OmiscidTrace( "Could not open segment %8.8x\n", SegKey );
            return false;
        }
    }
    // Get address of the segment
    SegAddress = (void *)shmat( iSegMem, 0, 0 );
    if ( SegAddress == NULL )
    {
        OmiscidTrace( "Could not open segment %8.8x (no access)\n", SegKey );
        Close();
        return false;
    }

#endif // WIN32

    // Here we have the Segment, so prepare it
    InitNewSharedMemSegment( GrantWriteAccessToOthers );

    return true;
}

bool SharedMemSegment::Open( unsigned int SegKey, bool ReadAndWriteAccess /* = false */ )
{
    // Set name of this segment
    SetName( SegKey );

#ifdef WIN32

    // Open the file map
    hSegMemFile = OpenFileMapping( FILE_MAP_READ | FILE_MAP_WRITE, FALSE, Name.GetStr() );
    if ( hSegMemFile == 0 )
    {
        OmiscidTrace( "Could not open segment %8.8x\n", SegKey );
        return false;
    }

    DWORD DesiredAccess = FILE_MAP_READ;
    // Do we want also a write access
    if ( ReadAndWriteAccess == true )
    {
        DesiredAccess |= FILE_MAP_WRITE;
    }

    // Get adress of the shared memory segment
    // We will check later if we have realy access
    SegAddress = (void*)MapViewOfFile( hSegMemFile, DesiredAccess, 0, 0, 0 );
    if ( SegAddress == NULL )
    {
        OmiscidTrace( "Could not open segment %8.8x (no access)\n", SegKey );
        Close();
        return false;
    }
#else
    // open a previously created shared segment or fail
    iSegMem = shmget( Name, 0, 0 );
    if ( iSegMem == -1 )
    {
        OmiscidTrace( "Could not open segment %8.8x\n", SegKey );
        return false;
    }

    int DesiredAccess;
    // Do we want also a write access
    if ( ReadAndWriteAccess == false )
    {
        DesiredAccess = SHM_RDONLY;
    }
    else
    {
        DesiredAccess = 0;
    }

    // Get address of the segment
    SegAddress = (void *)shmat( iSegMem, 0, DesiredAccess );
    if ( SegAddress == NULL )
    {
        OmiscidTrace( "Could not open segment %8.8x (no access)\n", SegKey );
        Close();
        return false;
    }
#endif // WIN32

    // Here we have the Segment, so prepare it
    return GetSharedMemSegmentAccess( ReadAndWriteAccess );
}

void SharedMemSegment::Close()
{
    // Free ressources
#ifdef WIN32
    if ( SegAddress != NULL )
    {
        UnmapViewOfFile( SegAddress );
    }
    if ( hSegMemFile != NULL )
    {
        CloseHandle( hSegMemFile );
    }
#else
    // Say to the system that this share can be destroy
    // if no one is using it
    if ( iSegMem != -1 )
    {
        shmctl(iSegMem, IPC_RMID, NULL );
    }
    if ( SegAddress != NULL )
    {
        shmdt( SegAddress );
    }
#endif // WIN32

    // Unset members
    SegmentKey    = 0;
    SegAddress    = NULL;
    AccessFlags    = NULL;
    UserAddress    = NULL;

#ifdef WIN32
    Name.Empty();
    hSegMemFile = NULL;
#else
    Name    = (key_t)0;
    iSegMem = -1;
#endif
}
