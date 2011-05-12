/**
 * \file Messaging/StructuredMethodCall.h
 * \ingroup Messaging
 * \brief Definition of Structured Method Call type and function
 */
#ifndef __STRUCTURED_METHOD_CALL_H__
#define __STRUCTURED_METHOD_CALL_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions, 
// C4290 is just informing you that other exceptions may still be throw from 
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
#endif

#include <System/SimpleString.h>

#include <Messaging/SerializeManager.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredParameters.h>
#include <Messaging/StructuredMessageException.h>

namespace Omiscid {
	class StructuredMethodCall;

/**
 * \class StructuredMethodCall StructuredMethodCall.h Messaging/StructuredMethodCall.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method Call
 *
 * contain the data about a Structured Method Call:
 * 
 * - to be detailed :)
 *
 * \author R�mi Barraquand
 */
class StructuredMethodCall : public StructuredMessage {
public:
  /** \brief Constructor
  */
  StructuredMethodCall();

  /** \brief Constructor
  *
  * \param val [in] Copy constructor
  */
  StructuredMethodCall( const StructuredMessage& Obj) throw( StructuredMessageException );

  /** \brief Constructor
  *
  * \param val [in] Copy constructor
  */
  StructuredMethodCall( const StructuredMethodCall& Obj);

  /** \brief Constructor
  *
  * \param msg [in] Default constructor
  */
  StructuredMethodCall( const SimpleString& MethodName, const StructuredParameters& Params);

  /** \brief Constructor
   *
   */
  template<typename TP1>
  StructuredMethodCall( const SimpleString& MethodName, TP1 p1);

  /** \brief Constructor
   *
   */
  template<typename TP1, typename TP2>
  StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2);

  /** \brief Constructor
   *
   */
  template<typename TP1, typename TP2, typename TP3>
  StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2, TP3 p3);

  /** \brief Constructor
   *
   */
  template<typename TP1, typename TP2, typename TP3, typename TP4>
  StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2, TP3 p3, TP4 p4);

  /** \brief Destructor
  */
  ~StructuredMethodCall();

  /** \brief Overload of the = operator
  */
  const StructuredMessage& operator= ( const StructuredMethodCall& Msg);

  /** \brief Get the method name
  * \return the method name.
  */
  const SimpleString& GetMethodName() const;

  /** \brief Get the parameters
  * \return the parameters.
  */
  const StructuredParameters& GetParameters() const;

  /** \brief Set the method name
  * \param MethodName the method name.
  */
  void SetMethodName( const SimpleString& MethodName );

  /** \brief Set the parameters
  * \param Params the parameters.
  */
  void SetParameters( const StructuredParameters& Params );

  /** \brief Check if a structured message is formated as a method call.
  * \param Msg The structured message to check.
  * \return return true if well formated, false otherwise.
  */
  static bool IsMethodCall( const StructuredMessage& Msg);

  /** \brief Build from structuredMessage
  * \param Obj [in] the structuredMessage to build the method call from.
  */
  void BuildFromStructuredMessage( const StructuredMessage& Msg ) throw( StructuredMessageException );

protected:
  // Change the visibility of those function (nice hack baby !!!!)
  using StructuredMessage::Pop;
  using StructuredMessage::Put;
  using StructuredMessage::Has;
  using StructuredMessage::Get;

private:
  /** \brief Set MethodName and Parameters
  * \param MethodName the name of the method.
  * \param Params the parameters.
  */
  void Init( const SimpleString& MethodName, const StructuredParameters& Params );

  SimpleString MethodName;
  StructuredParameters Params;
};

// Implementation
template<typename TP1>
StructuredMethodCall::StructuredMethodCall( const SimpleString& MethodName, TP1 p1)
{
	StructuredParameters params;
	params.AddParameter(p1);
	Init(MethodName, params);
}

template<typename TP1, typename TP2>
StructuredMethodCall::StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2)
{
	StructuredParameters params;
	params.AddParameter(p1);
	params.AddParameter(p2);
	Init(MethodName, params);
}

template<typename TP1, typename TP2, typename TP3>
StructuredMethodCall::StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2, TP3 p3)
{
	StructuredParameters params;
	params.AddParameter(p1);
	params.AddParameter(p2);
	params.AddParameter(p3);
	Init(MethodName, params);
}

template<typename TP1, typename TP2, typename TP3, typename TP4>
StructuredMethodCall::StructuredMethodCall( const SimpleString& MethodName, TP1 p1, TP2 p2, TP3 p3, TP4 p4)
{
	StructuredParameters params;
	params.AddParameter(p1);
	params.AddParameter(p2);
	params.AddParameter(p3);
	params.AddParameter(p4);
	Init(MethodName, params);
}

} // Omiscid::Messaging

#endif //__STRUCTURED_METHOD_CALL_H__

