/**
 * @file TrackingMemoryLeaks.h
 * @ingroup System
 * @brief A way to discover memory leaks in Omiscid code.
 */

#ifndef __TRACKING_MEMORY_LEAKS_H__
#define __TRACKING_MEMORY_LEAKS_H__

// Only when asked
#ifdef TRACKING_MEMORY_LEAKS

#if defined WIN32
	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif
#endif

#include <memory>

	#ifdef WIN32
		#pragma warning(disable : 4290)	// warning about throw specification in new/delete declaration...
	#endif

	void * operator new( size_t size ) throw (std::bad_alloc);
	void * operator new[]( size_t size ) throw (std::bad_alloc);
	void operator delete( void *p ) throw ();
	void operator delete[]( void *p ) throw ();

	void * operator new( size_t size, int Line, char * File ) throw (std::bad_alloc);
	void * operator new[]( size_t size, int Line, char * File ) throw (std::bad_alloc);
	void operator delete( void *p, int Line, char * File ) throw ();
	void operator delete[]( void *p, int Line, char * File ) throw ();


namespace Omiscid {

class TrackMemoryLeaks
{
public:
	TrackMemoryLeaks();
	virtual ~TrackMemoryLeaks();

	static void DumpUnfreed();

private:
	static void StartTrackingMemory();
	static void StopTrackingMemory();
};

/** @brief Initialise Tacking system */
// void TrackMemoryLeaksInit();

} // namespace Omiscid

#endif	// defined  TRACKING_MEMORY_LEAKS

#endif	// __TRACKING_MEMORY_LEAKS_H__
