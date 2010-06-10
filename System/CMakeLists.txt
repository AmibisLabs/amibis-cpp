INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(OmiscidSystem_SRCS
	System/AtomicCounter.h
	System/AtomicReentrantCounter.h
	System/AutoDelete.h
	System/ConfigSystem.h
	System/DateAndTime.h
	System/ElapsedTime.h
	System/Event.h
	System/LockManagement.h
	System/MultipleReferencedData.h
	System/Mutex.h
	System/MutexedSimpleList.h
	System/Portage.h
	System/RecycleSimpleList.h
	System/ReentrantMutex.h
	System/SharedMemSegment.h
	System/SimpleException.h
	System/SimpleList.h
	System/SimpleListException.h
	System/SimpleString.h
	System/Socket.h
	System/SocketException.h
	System/TemporaryMemoryBuffer.h
	System/Thread.h
	System/TrackingMemoryLeaks.h
	AtomicCounter.cpp
	AtomicReentrantCounter.cpp
	ConfigSystem.cpp
	DateAndTime.cpp
	ElapsedTime.cpp
	Event.cpp
	LockManagement.cpp
	MultipleReferencedData.cpp
	Mutex.cpp
	MutexedSimpleList.cpp
	Portage.cpp
	RecycleSimpleList.cpp
	ReentrantMutex.cpp
	SharedMemSegment.cpp
	SimpleException.cpp
	SimpleList.cpp
	SimpleListException.cpp
	SimpleString.cpp
	Socket.cpp
	SocketException.cpp
	TemporaryMemoryBuffer.cpp
	Thread.cpp
	TrackingMemoryLeaks.cpp
)
ADD_LIBRARY(OmiscidSystem SHARED ${OmiscidSystem_SRCS})
TARGET_LINK_LIBRARIES(OmiscidSystem ${CMAKE_THREAD_LIBS_INIT})
SET_TARGET_PROPERTIES(OmiscidSystem PROPERTIES 
    VERSION ${OMISCID_VERSION} 
    SOVERSION ${OMISCID_SO_VERSION})

CONFIGURE_FILE(OmiscidSystem-config.in.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/OmiscidSystem-config @ONLY)

INSTALL(TARGETS OmiscidSystem
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
INSTALL(DIRECTORY System DESTINATION include FILES_MATCHING PATTERN "*.h")
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/OmiscidSystem-config 
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

