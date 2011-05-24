/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/AccumulatorStructuredMessage.h
 * @ingroup Examples
 * @brief Demonstration of an accumulator server with structured message. Header file for the object to serialize.
 *
 * @author Dominique Vaufreydaz
 */

/**
 * @file Examples/ClientForAccumulator.h
 * @ingroup Examples
 * @brief Header of Demonstration of an Accumulator client.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __ACCUMULATOR_STRUCTURED_MESSAGE_H__
#define __ACCUMULATOR_STRUCTURED_MESSAGE_H__

// Standard includes
#include <Messaging/Serializable.h>

namespace Omiscid {

class OperationToCompute : public Serializable
{
public:

	OperationToCompute()
	{
	}

	~OperationToCompute()
	{
	}

	enum OperationType { Equality = 0, Addition = 1, Substraction = 2, Multiplication = 3, Division = 4 };
	int Operation;
	float fRightOperand;

	void DeclareSerializeMapping()
	{
		AddToSerialization( "Operation", Operation );
		AddToSerialization( "Operand", fRightOperand );
	}

};

} // namespace Omiscid

#endif // __ACCUMULATOR_STRUCTURED_MESSAGE_H__

