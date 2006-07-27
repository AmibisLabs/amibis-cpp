/**
 * @file TrackingMemoryLeaks.h
 * @ingroup System
 * @brief A way to discover memory leaks in Omiscid code.
 */

#ifndef __TRACKING_MEMORY_LEAKS_H__
#define __TRACKING_MEMORY_LEAKS_H__

#if defined WIN32 || defined _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <memory>

namespace Omiscid {

void StartTrackingMemoryLeaks();
void StopTrackingMemoryLeaks();

void AddMemoryBlock(void* addr,  size_t asize,  const char *fname, int lnum);
void RemoveMemoryBlock(void* addr);


} // namespace Omiscid

// Only in debug mode
#ifndef DEBUG
	#ifdef WIN32
		#pragma warning(disable : 4291)
	#endif

	void * operator new( size_t size, int line, const char *file );
	void * operator new[]( size_t size, int line, const char *file );
	void operator delete( void *p );
	void operator delete[]( void *p );

	#ifdef OMISCID_NEW
		#undef OMISCID_NEW
	#endif
	
	#define OMISCID_NEW new( __LINE__, __FILE__ )
#else

	#define OMISCID_NEW new
	
#endif

#define new OMISCID_NEW

#endif	// __TRACKING_MEMORY_LEAKS_H__
