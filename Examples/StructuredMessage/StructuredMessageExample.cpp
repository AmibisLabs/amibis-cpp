/*
 * StructuredMessageExample.cpp
 *
 *  Created on: 17 mai 2010
 *      Author: Amaury Negre
 */

#include <string>
#include <iostream>

#include <Messaging/Messaging.h>

class Info
{
public:
  Info(bool b) : b(b) {}
  Info(const Info & i) : b(i.b) {}
  bool b;
};

void Save(Omiscid::Messaging::StructuredMessage& msg, const Info& info)
{
  msg.Put("Terran", info.b);
}

void Load(Omiscid::Messaging::StructuredMessage& msg, Info& info)
{
  msg.Get("Terran", info.b);
}

class Person
{
public:
  std::string name;
  int age;
  float height;
  Info info;

  Person(const std::string& n="", int a=0, float h=0., bool terran=true)
	: name(n)
	, age(a)
	, height(h)
	, info(terran)
  {}

  Person(const Person& p)
	: name(p.name)
	, age(p.age)
	, height(p.height)
	, info(p.info)
  {}

  void Save(Omiscid::Messaging::StructuredMessage& msg) const
  {
	msg.Put("name", name);
	msg.Put("age", age);
	msg.Put("height", height);
	msg.Put("info", info);
  }

  void Load(const Omiscid::Messaging::StructuredMessage& msg)
  {
	msg.Get("name", name);
	msg.Get("age", age);
	msg.Get("height", height);
	msg.Get("info", info);
  }
};

typedef struct my_struct
{
  int x;
  float y;
} MyStruct;

void Save(Omiscid::Messaging::StructuredMessage& msg, const MyStruct& s)
{
  msg.Put("x", s.x);
  msg.Put("y", s.y);
}

void Load(Omiscid::Messaging::StructuredMessage& msg, MyStruct& s)
{
  msg.Get("x", s.x);
  msg.Get("y", s.y);
}

using namespace Omiscid;
using namespace Omiscid::Messaging;
using namespace std;

int main(int argc, char **argv)
{
  StructuredMessage msg;
  Person p("Amaury", 27, 1.82);
  msg.Put("Person", p);

  vector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);

  msg.Put("Vec", vec);

  vector<float> vecf;
  vecf.push_back(0.1);
  vecf.push_back(0.2);
  vecf.push_back(0.3);

  msg.Put("Vecf", vecf);

  vector<Person> vecp;
  vecp.push_back(Person("Dominique", 52, 1.5));
  vecp.push_back(Person("Remi", 26, 1.75, false));

  msg.Put("VecP", vecp);

  MyStruct my_struct = {1, 1.1};
  msg.Put("MyStruct", my_struct);

  std::cout << msg.Encode() << std::endl;

  Person p2;
  msg.Get("Person", p2);

  vector<int> vec2;
  msg.Get("Vec", vec2);

  vector<Person> vecp2;
  msg.Get("VecP", vecp2);

  msg.Clear();

  msg.Put("Person", p2);
  msg.Put("Vec2", vec2);
  msg.Put("VecP", vecp2);
  std::cout << msg.Encode() << std::endl;

//   msg.Put("Person2", p2);
//   std::cout << msg.Encode() << std::endl;

  SimpleString str = msg.Encode();

  StructuredMessage msg2;
  StructuredMessage::DecodeStructuredMessage(str, msg2);
  std::cout << msg2.Encode() << std::endl;

}

