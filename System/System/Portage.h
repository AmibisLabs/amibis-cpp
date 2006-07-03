#ifndef __PORTAGE_H__
#define __PORTAGE_H__

#include <System/Config.h>
#include <System/SimpleString.h>

namespace Omiscid {

#ifdef WIN32

// Time function port under Windows
int gettimeofday(struct timeval*tv,struct timezone*tz ); // tz is ignored on windows plateforms

// random port
int random();

#endif	// ifdef WIN32

// RandomInit()
void RandomInit();

// Retrieve the logged username
SimpleString GetLoggedUser();

} // namespace Omiscid

#endif	// __PORTAGE_H__
