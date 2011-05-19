/**
 * @file Examples/BrowsingTest.h
 * @ingroup Examples
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __BROWSING_TEST_H__
#define __BROWSING_TEST_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

namespace Omiscid {

#ifdef OMISCID_RUNING_TEST
// Call test in a separate function
extern int DoBrowsingTest(int argc, char*argv[] );
#endif // OMISCID_RUNING_TEST

} // namespace Omiscid

#endif // __BROWSING_TEST_H__
