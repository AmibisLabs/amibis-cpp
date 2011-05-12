/**
 * \file Messaging/StructuredParameters.h
 * \ingroup Messaging
 * \brief Definition of Structured Parameters type and function
 */
#ifndef __STRUCTURED_PARAMETERS_H__
#define __STRUCTURED_PARAMETERS_H__

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
  class StructuredParameters;
  class StructuredMessageException;

/**
 * \class StructuredParameters StructuredParameters.h Messaging/StructuredParameters.h
 * \ingroup Messaging
 * \brief Group Data about a structured parameters
 *
 * contain the data about a structured parameters :
 * 
 * - to be detailed :)
 *
 * \author R�mi Barraquand
 */
class StructuredParameters {
public:
  /** \brief Constructor
  */
  StructuredParameters();

  /** \brief Constructor
  * \param Obj [in] Copy constructor.
  */
  StructuredParameters(const json_spirit::Array& Obj);

  /** \brief Destrutor
  */
  ~StructuredParameters();

  /** \brief Get parameters array
  * \return An arrays of params
  */
  const json_spirit::Array& GetArray() const;

  /** \brief Add parameter
  * \param Param the param to add in the structure
  */
  void AddParameter( json_spirit::Value Val );

  /** \birel Add parameter of any type
   * \param Param the param to add in the structure
   */
  template< class C >
    void AddParameter( const C & Param );

  /** \brief Get number of parameters
  * \return The number of parameters
  */
  int GetNumberOfParameters() const;

  /** \brief Get parameter of type int
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, int& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type SimpleString
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, SimpleString& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type Value
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, json_spirit::Value& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type Object
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, json_spirit::Object& Val ) const throw( StructuredMessageException );
  
  /** \brief Get parameter of type Array
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, json_spirit::Array& Val ) const throw( StructuredMessageException );
  
  /** \brief Get parameter of type bool
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, bool& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type double
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, double& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type float
  * \param Index [in] the index of the parameter to get.
  * \param Val [out] the value of the parameter
  */
  void GetParameter( int Index, float& Val ) const throw( StructuredMessageException );

  /** \brief Get parameter of type vector
    * \param Index [in] the index of the parameter to get.
    * \param Val [out] the value of the parameter
    */
  template<class T>
  void GetParameter( int Index, std::vector<T>& Val) const throw( StructuredMessageException );

  template<class C>
  void GetParameter( int Index, C& Val) const throw( StructuredMessageException );
  
  /** \brief Output as string for debug purpose
  * \return A string representing the parameters
  */
  SimpleString ToString();

private:
  json_spirit::Array Params;
};

/* Implementation */

template<class C>
  void StructuredParameters::AddParameter( const C & Param )
{
  StructuredMessage msg;
  msg.Put( "p", Param );  
  AddParameter(msg.Get("p"));
}

template<class T>
  void StructuredParameters::GetParameter( int Index, std::vector<T>& Val) const throw( StructuredMessageException )
  {
	 if( Index>= 0 && Index < GetNumberOfParameters() ) {
	    if( this->Params[Index].type() == json_spirit::array_type ) {
	      const json_spirit::Array& array = Params[Index].get_array();
	      Val.clear();
	      for(json_spirit::Array::const_iterator it = array.begin();
	    	it != array.end();
	    	++it)
	      {
	    	  T v;
	    	  it->get_val(v);
	    	  Val.push_back(v);
	      }
	    } else {
	      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
	    }
	  } else {
	    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
	  }

  }

template<class C>
void StructuredParameters::GetParameter( int Index, C& Val) const throw( StructuredMessageException )
{
  json_spirit::Value value;
  GetParameter(Index, value);
  StructuredMessage msg;
  msg.Put("p", value);
  msg.Get("p", Val);
}

} // Omiscid

#endif // __STRUCTURED_PARAMETERS_H__
