//      -*- C++ -*-

/*! @file RaviControlServer.h
 * @brief Header of the common classes and values for the PRIMA Service package
 *
 * Control Server for Ravi with callback for scheme
 * @date 2004-2005
 */

#ifndef RAVI_CONTROLSERVER_H
#define RAVI_CONTROLSERVER_H

#ifndef FOR_LIB

#include <ServiceControl/ControlServer.h>


/**
 * @class RaviControlServer RaviControlServer.h ServiceControl/RaviControlServer.h
 * @brief Implementation of ControlServer for Ravi
 *
 * Reimplements the methods Connect and ModifVariable called to answer to client request.
 * This implementation call scheme callback functions ConnectSchemeFct and 
 * ModifVariableSchemeFct.
 * @author Sebastien Pesnel
 */
class RaviControlServer : public ControlServer
{
 public:
  /** @brief Constructor
   * @param service_name name for the service
   */
  RaviControlServer(const char* service_name = "Control");

  /** @brief Access to properties. useful in Ravi
   * @return Properties */
  ServiceProperties* GetProperties();

 protected:
  virtual void Connect(const SimpleString& host, int port, bool tcp, InOutputAttribut* ioa);
  virtual void ModifVariable(int length, const unsigned char* buffer, int status, VariableAttribut* va);
};



//#ifdef RAVI

/*Ravi declare ((scheme-function ConnectSchemeFct))   */
void ConnectSchemeFct(const char* host, int port, bool tcp, InOutputAttribut* ioa, RaviControlServer* ctrl);

/*Ravi declare ((scheme-function ModifVariableSchemeFct ))   */
void ModifVariableSchemeFct(int length, const char* buffer, int status, VariableAttribut* va, RaviControlServer* ctrl);

//#endif



////////// inline methods /////////////
#ifndef RAVI_INTERFACE

inline void RaviControlServer::Connect(const SimpleString& host, int port, bool tcp, InOutputAttribut* ioa)
{
  ConnectSchemeFct(host.GetStr(), port, tcp, ioa, this);
}
inline void RaviControlServer::ModifVariable(int length, const unsigned char* buffer, int status, VariableAttribut* va)
{
  ModifVariableSchemeFct(length, (const char*)buffer, status, va, this);
}

inline ServiceProperties* RaviControlServer::GetProperties()
{ return &Properties; }

#endif /* RAVI_INTERFACE */

#endif /* FOR_LIB */

#endif /* RAVI_CONTROLSERVER_H */
