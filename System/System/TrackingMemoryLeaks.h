#ifndef __TRACKING_MEMORY_LEAKS_H__
#define __TRACKING_MEMORY_LEAKS_H__


void StartTrackingMemoryLeaks();
void StopTrackingMemoryLeaks();

// Only in debug mode
#ifdef DEBUG

	#pragma warning(disable : 4291)

	void AddTrack(void* addr,  unsigned int asize,  const char *fname, int lnum);
	void RemoveTrack(void* addr);

	inline void * __cdecl operator new(unsigned int size, const char *file, int line )
	{
		void *ptr = (void *)malloc(size);
		AddTrack(ptr, size, file, line);
		return(ptr);
	};

	inline void __cdecl operator delete(void *p)
	{
		RemoveTrack(p);
		free(p);
	};

	#define DEBUG_NEW new(__FILE__, __LINE__)

#else

	#define DEBUG_NEW new
	
#endif

#define new DEBUG_NEW


#endif	// __TRACKING_MEMORY_LEAKS_H__
