/*
 * Access.h
 *
 *  Created on: 17 mai 2010
 *      Author: Amaury Negre
 */

#ifndef ACCESS_H_
#define ACCESS_H_

namespace Omiscid {

class StructuredMessage;

class Access {
public:
  template<class T>
  static void Save( StructuredMessage& Msg, const T& t)
  {
	t.Save(Msg);
  }

  template<class T>
  static void Load( StructuredMessage& Msg, T& t)
  {
	t.Load(Msg);
  }
};

} // Omiscid

#endif /* ACCESS_H_ */
