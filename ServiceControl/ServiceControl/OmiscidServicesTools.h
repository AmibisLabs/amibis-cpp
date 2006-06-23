/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef _OMiSCID_SERVICES_H_
#define _OMiSCID_SERVICES_H_

#include <System/Portage.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/WaitForServices.h>

namespace Omiscid {

/** 
 * @class RegisterOmiscidService OmiscidServicesTools.h ServiceControl/OmiscidServices.h
 * @brief To Comment
 */
class RegisterOmiscidService : public RegisterService
{
public:
	RegisterOmiscidService( const char * FullName, uint16_t ePort, bool AutoRegister = false );
 	RegisterOmiscidService( const char * ServiceName, const char * Domain, uint16_t ePort, bool AutoRegister = false );

	void SetOwner();

private:
	void Init( bool AutoRegister );
};

class WaitForOmiscidServices : public WaitForServices, public CommonServiceValues
{
public:
	WaitForOmiscidServices();
	virtual ~WaitForOmiscidServices(void);

	int NeedService( const char * eName, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL )
	{
		return WaitForServices::NeedService( eName, OmiscidServiceDnsSdType, eCallBack, eUserData );
	}
};

} // namespace Omiscid

#endif /* _OMiSCID_SERVICES_H_ */
