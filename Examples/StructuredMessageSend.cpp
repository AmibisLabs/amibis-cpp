/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/StructuredMessageSend.cpp
 * @ingroup Examples
 * @brief Demonstration of a Strustured messages. You can also see Examples/ClientAccumulatorWithStrusturedMessage.cpp
 * and AccumulatorWithStructuredMessage.cpp.
 * @author Dominique Vaufreydaz
 */

// Standard complete includes
#include <Omiscid.h>

#include <iostream>
using namespace std;

using namespace Omiscid;

#ifdef OMISCID_RUNING_TEST
#include "TimeoutProg.h"

/* @brief main program entry for the Accumulator. No need to give parameter */
int Omiscid::DoStructuredMessagesTest( int argc, char*argv[] )
{
	TimeoutProg ExitIn2Minutes;

#else

/* @brief main program entry for the Accumulator. No need to give parameter */
int main( int argc, char*argv[] )
{

#endif // OMISCID_RUNING_TEST

	// Serialization of simple value, int in this case
	int i = -12;

	// Create a structured message from an int and output it !
	StructuredMessage SerializeMessageOfInt(i);
	cerr << "Serialize of i (" << i << ") = " << SerializeMessageOfInt << endl;
	// Another way to do it
	i = 128;
	SerializeMessageOfInt = Serialize(i);
	cerr << "Serialize of i (" << i << ") = " << SerializeMessageOfInt << endl;

#if 0
	StructuredMessage SerializationOfObject = ObjectToSerialize.Serialize();

	fprintf( stderr, "%s\n", ((SimpleString)SerializationOfObject).GetStr() );

	ObjectToSerialize.MyInt = 0;
	fprintf( stderr, "MyInt = %d\n", ObjectToSerialize.MyInt );

	ObjectToSerialize.Unserialize( SerializationOfObject );

	fprintf( stderr, "MyInt = %d\n", ObjectToSerialize.MyInt );

	SimpleList<int> My_int_List;
	for( i = 0; i < 10; i++ )
	{
		My_int_List.AddTail(i);
	}

	StructuredMessage SerializationOfObject2;
	SerializationOfObject2.Put( "Test", Serialize( My_int_List ) );
	fprintf( stderr, "%s\n", ((SimpleString)SerializationOfObject2).GetStr() );

	My_int_List.Empty();
	Unserialize( SerializationOfObject2.FindAndGetValue( "Test" ), My_int_List );
	fprintf( stderr, "Unserialized value of My_int_List\n" );
	for( My_int_List.First(); My_int_List.NotAtEnd(); My_int_List.Next() )
	{
		fprintf( stderr, "%d\n", My_int_List.GetCurrent() );
	}

#endif

#ifndef OMISCID_RUNING_TEST
	Event ForEver;
	ForEver.Wait();
#endif

	return 0;
}

