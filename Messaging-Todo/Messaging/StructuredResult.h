/**
 * \file Messaging/StructuredResult.h
 * \ingroup Messaging
 * \brief Definition of Structured Result type and function
 */
#ifndef __STRUCTURED_RESULT_H__
#define __STRUCTURED_RESULT_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions,
// C4290 is just informing you that other exceptions may still be throw from
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
#endif

#include <System/SimpleString.h>

#include <Messaging/SerializeManager.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredMessageException.h>

namespace Omiscid {
  class StructuredResult;

/**
 * \class StructuredResult StructuredResult.h Messaging/StructuredResult.h
 * \ingroup Messaging
 * \brief Group Data about a structured result
 *
 * contain the data about a structured result :
 *
 * - to be detailed :)
 *
 * \author Rémi Barraquand
 */
class StructuredResult : public StructuredMessage
{
public:
  /** \brief Constructor
  */
  StructuredResult();

  /** \brief Constructor
  * \Param Msg Copy constructor.
  */
  StructuredResult( const StructuredMessage& Msg) throw( StructuredMessageException );

  /** \brief Destrutor
  */
  ~StructuredResult();

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( const SimpleString& Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( const json_spirit::Value& Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( const json_spirit::Object& Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( const json_spirit::Array& Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( bool Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( int Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( double Val );

//   /** \brief Set the result
//   * \param Val the value to set
//   */
//   void SetResult( float Val );

  /** \brief Set the result
  * \param Val the value to set
  */
  template <typename T>
	void SetResult( const T & Val );

  /** \brief Set the error
  * \param Error the error message.
  */
  void SetError( const SimpleString& Error );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( SimpleString& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( json_spirit::Value& Val ) const;

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( json_spirit::Object& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( json_spirit::Array& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( bool& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( int& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( double& Val ) const throw( StructuredMessageException );

//   /** \brief Get the result
//   * \param Val the value to set
//   */
//   void GetResult( float& Val ) const throw( StructuredMessageException );

  template <typename T>
	void GetResult( T & Val ) const throw( StructuredMessageException );

  /** \brief Get error if it has.
  * \return the error message.
  */
  const SimpleString& GetError() const;

  /** \brief Get if their is an error
  * \return true if their is an error, false otherwise.
  */
  bool HasError() const;

  /** \brief Check if a structured message is formated as a result.
  * \param Msg The structured message to check.
  * \return return true if well formated, false otherwise.
  */
  static bool IsResult( const StructuredMessage& Msg);

  /** \brief Build from structuredMessage
  * \param Msg [in] the structuredMessage to build the result from.
  */
  void BuildFromStructuredMessage( const StructuredMessage& Msg ) throw( StructuredMessageException );

protected:
  // Change the visibility of those function (nice hack baby !!!!)
  using StructuredMessage::Pop;
  using StructuredMessage::Put;
  using StructuredMessage::Has;
  using StructuredMessage::Get;

  SimpleString Error;
  //json_spirit::Value Result;
};

/* Implementation */
  template <typename T>
	void StructuredResult::SetResult( const T & Val )
  {
	Set("result", Val);
  }

  template <typename T>
	void StructuredResult::GetResult( T & Val ) const throw( StructuredMessageException )
  {
	Get("result", Val);
  }

} // Omiscid

#endif // __STRUCTURED_PARAMETERS_H__
