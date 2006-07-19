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

#include <stdio.h>
#include <stdlib.h>

namespace Omiscid {

void StartTrackingMemoryLeaks();
void StopTrackingMemoryLeaks();

void AddMemoryBlock(void* addr,  size_t asize,  const char *fname, int lnum);
void RemoveMemoryBlock(void* addr);

} // namespace Omiscid

// Only in debug mode
#ifdef DEBUG
	// Only for non Windows plateforme
	#if defined WIN32 || defined _WIN32
		#define OperatorCallConvention __cdecl 
	#else
		#define OperatorCallConvention
	#endif

	#pragma warning(disable : 4291)

	inline void * OperatorCallConvention operator new(size_t size, const char *file, int line )
	{
		void *ptr = (void *)malloc(size);
		Omiscid::AddMemoryBlock(ptr, size, file, line);
		return(ptr);
	};

	inline void * OperatorCallConvention operator new[](size_t size, const char *file, int line )
	{
		void *ptr = (void *)malloc(size);
		Omiscid::AddMemoryBlock(ptr, size, file, line);
		return(ptr);
	};

	inline void OperatorCallConvention operator delete(void *p)
	{
		Omiscid::RemoveMemoryBlock(p);
		free(p);
	};

	inline void OperatorCallConvention operator delete[](void *p)
	{
		Omiscid::RemoveMemoryBlock(p);
		free(p);
	};

	#define DEBUG_NEW new(__FILE__, __LINE__)

#else

	#define DEBUG_NEW new
	
#endif

#define new DEBUG_NEW


#endif	// __TRACKING_MEMORY_LEAKS_H__
