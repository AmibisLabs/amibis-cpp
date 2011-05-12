/**
 * @defgroup Examples Examples of how to use Strustured Messages in OMiSCID.
 *
 */

 /**
 * @file Examples/StructuredMessageSend.h
 * @ingroup Examples
 * @brief Demonstration of an structured messages. Header file.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __STRUCTURED_MESSAGE_SEND_H__
#define __STRUCTURED_MESSAGE_SEND_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

namespace Omiscid {

/**
 * @class ClassToSerialize
 * @ingroup Examples
 * @brief this class is used to managed an accumulator service. Is is also used
 *        to monitor incomming activity from intput connector of the service
 */

class ClassIntToSerialize : public Serializable
{
public:

	int MyInt;
	
	ClassIntToSerialize()
	{
		MyInt = -1;
	}

	virtual ~ClassIntToSerialize() {};

	void DeclareSerializeMapping()
	{
		AddToSerialization( "MyInt", MyInt );
	}
};

class ClassIntListToSerialize : public Serializable
{
public:

	SimpleList<int> MyInts;
	
	ClassIntListToSerialize()
	{
		for( int i = 0; i < 10; i++ )
		{
			MyInts.AddTail(i);
		}
	}

	~ClassIntListToSerialize() {};

	void DeclareSerializeMapping()
	{
		AddToSerialization( "MyInts", MyInts );
	}
};


#ifdef OMISCID_RUNING_TEST
// Call test in a separate function
extern int DoStructuredMessageTest(int argc, char*argv[] );
#endif // OMISCID_RUNING_TEST

} // namespace Omiscid

#endif // __STRUCTURED_MESSAGE_SEND_H__
