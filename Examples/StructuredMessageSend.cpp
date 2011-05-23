/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/Accumulator.cpp
 * @ingroup Examples
 * @brief Demonstration of an accumulator server.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include "StructuredMessageSend.h"

#include <System/LockManagement.h>

#include <iostream>
using namespace std;

using namespace Omiscid;

#ifdef OMISCID_RUNING_TEST
#include "TimeoutProg.h"

/* @brief main program entry for the Accumulator. No need to give parameter */
int Omiscid::DoAccumulatorTest( int argc, char*argv[] )
{
	TimeoutProg ExitIn2Minutes;

#else

/* @brief main program entry for the Accumulator. No need to give parameter */
int main( int argc, char*argv[] )
{

#endif // OMISCID_RUNING_TEST

	int i = -12;
	ClassIntToSerialize ObjectToSerialize;

	// StructuredMessage SerializationOfObject3(i);
	// fprintf( stderr, "%s\n", SerializationOfObject3.EncodeStructuredMessage().GetStr() );


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

	Event ForEver;
	ForEver.Wait();

	return 0;
}
