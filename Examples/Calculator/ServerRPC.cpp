#include "ServerRPC.h"

Calculator::Calculator()
{
}

void Calculator::Add(const StructuredParameters& Param, StructuredResult& Result)
{
  int n = Param.GetNumberOfParameters();
  if( n == 2 ) {
    json_spirit::Value v1, v2;
    Param.GetParameter(0, v1);
    Param.GetParameter(1, v2);

    if( v1.type() == v2.type() && v1.type() == json_spirit::int_type ) {
      Result.SetResult( v1.get_int() + v2.get_int() );
      return;
    } else if( v1.type() == v2.type() && v1.type() == json_spirit::real_type ) {
      Result.SetResult( v1.get_real() + v2.get_real() );
      return;
    }
  }

  // else it is an error
  Result.SetError("Bad Arguments Type.");
}

void Calculator::Multiply(const StructuredParameters& Param, StructuredResult& Result)
{
  int n = Param.GetNumberOfParameters();
  if( n == 2 ) {
    json_spirit::Value v1, v2;
    Param.GetParameter(0, v1);
    Param.GetParameter(1, v2);

    if( v1.type() == v2.type() && v1.type() == json_spirit::int_type ) {
      Result.SetResult( v1.get_int() * v2.get_int() );
      return;
    } else if( v1.type() == v2.type() && v1.type() == json_spirit::real_type ) {
      Result.SetResult( v1.get_real() * v2.get_real() );
      return;
    }
  }

  // else it is an error
  Result.SetError("Bad Arguments Type.");
}

std::string Calculator::Msg()
{
  return std::string("Calculator Message");
}

int Calculator::Opp( int a )
{
  return -a;
}

void Calculator::Nop( int a )
{
}

double Calculator::Mad(double x, double y, double z)
{
	return x*y+z;
}

float Calculator::Div( float x, float y)
{
  return x / y;
}

void Calculator::Nop2( int a, int b)
{
  return;
}

float Calculator::Sum( std::vector<float> vals)
{
	float s = 0.;
	for(std::vector<float>::const_iterator it = vals.begin();
		it != vals.end();
		++it)
	{
		s += *it;
	}
	return s;
}

Vec2 Calculator::Add2( Vec2 v1, Vec2 v2)
{
  return Vec2(v1.x+v2.x, v1.y+v2.y);
}
