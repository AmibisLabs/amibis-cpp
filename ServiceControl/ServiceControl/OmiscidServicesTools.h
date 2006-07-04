/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef __OMISCID_SERVICES_TOOLS_H__
#define __OMISCID_SERVICES_TOOLS_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/WaitForDnsSdServices.h>

namespace Omiscid {

/** 
 * @class RegisterOmiscidService OmiscidServicesTools.h ServiceControl/OmiscidServices.h
 * @brief To Comment
 */
class RegisterOmiscidService : public RegisterService
{
public:
	RegisterOmiscidService( const SimpleString FullName, uint16_t ePort, bool AutoRegister = false );
 	RegisterOmiscidService( const SimpleString ServiceName, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false );

	void SetOwner();

private:
	void Init( bool AutoRegister );
};

class WaitForOmiscidServices : public WaitForDnsSdServices, public CommonServiceValues
{
public:
	WaitForOmiscidServices();
	virtual ~WaitForOmiscidServices(void);

	int NeedService( const SimpleString eName, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL )
	{
		return WaitForDnsSdServices::NeedService( eName, OmiscidServiceDnsSdType, eCallBack, eUserData );
	}
};

} // namespace Omiscid

#endif // __OMISCID_SERVICES_TOOLS_H__
