/**
 * @file Examples/Calculator/ServerRPC.h
 * @ingroup Example/Calculator
 * @brief Definition of the server RPC methods
 */
#ifndef __SERVER_RPC_H__
#define __SERVER_RPC_H__

// Standard includes
#include <Messaging/Messaging.h>

#include <string>
#include <vector>

using namespace Omiscid;
using namespace Omiscid::Messaging;

class Vec2
{
public:
  double x;
  double y;

  Vec2(double x, double y) : x(x), y(y) {}
  Vec2() : x(0.), y(0.) {}
  Vec2( const Vec2 & v ) : x(v.x), y(v.y) {}

  Vec2 & operator=(const Vec2 &v)
	{
	  x = v.x;
	  y = v.y;
	  return *this;
	}
  Vec2& operator+=(const Vec2 &v)
	{
	  x+=v.x;
	  y+=v.y;
	  return *this;
	}

  void Save(Omiscid::Messaging::StructuredMessage& msg) const
	{
	  msg.Put("x", x);
	  msg.Put("y", y);
	}

  void Load(Omiscid::Messaging::StructuredMessage& msg)
	{
	  msg.Get("x", x);
	  msg.Get("y", y);
	}
};

/**
 * @class Calculator ServerRPC.h Example/Calculator/ServerRPC.h
 * @ingroup Example/Calculator
 * @brief Definition of a Calculator
 *
 * @author R�mi Barraquand
 */
class Calculator
{
public:
  /** @brief Constructor
  */
  Calculator();

  /** @brief Add two numbers
  * @param Param [in] the structured parameters
  * @param Result [out] the structured results
  */
  void Add(const StructuredParameters& Param, StructuredResult& Result);

  /** @brief Multiply two numbers
  * @param Param [in] the structured parameters
  * @param Result [out] the structured results
  */
  void Multiply(const StructuredParameters& Param, StructuredResult& Result);

  std::string Msg();
  int Opp( int a );
  void Nop( int a );

  double Mad(double x, double y, double z);

  float Div( float x, float y);
  void Nop2( int a, int b);

  float Sum( std::vector<float> vals);

  Vec2 Add2( Vec2 v1, Vec2 v2);
};

#endif // __SERVER_RPC_H__
