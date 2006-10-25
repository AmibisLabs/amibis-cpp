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
		#pragma warning(disable : 4291)
	#endif

	void * operator new( size_t size ) throw ();
	void * operator new[]( size_t size ) throw ();
	void operator delete( void *p ) throw ();
	void operator delete[]( void *p ) throw ();

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

} // namespace Omiscid

#endif

#endif	// __TRACKING_MEMORY_LEAKS_H__
