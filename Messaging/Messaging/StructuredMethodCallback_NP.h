/**
 * \file Messaging/StructuredMethodCallBack_NP.h
 * \ingroup Messaging
 * \brief Definition of Structured Method CallBack fot N Parameters types
 */
#ifndef __STRUCTURED_METHOD_CALL_BACK_NP__
#define __STRUCTURED_METHOD_CALL_BACK_NP__

#include <Messaging/StructuredMethodCallback.h>

namespace Omiscid { namespace Messaging {


template <class Class, typename Result>
  class StructuredMethodCallback_0P : public StructuredMethodCallback<Class>
{
protected:
  /** \brief Define the type of a method callback */
  typedef Result (Class::*Method)();

public:
  StructuredMethodCallback_0P(Class* TheObject = NULL, typename StructuredMethodCallback_0P<Class, Result>::Method TheMethod = NULL);
  
  virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);
  
protected:
  Method TheMethod;
};


template <class Class, typename TP1, typename Result>
  class StructuredMethodCallback_1P : public StructuredMethodCallback<Class>
{
protected:
  /** \brief Define the type of a method callback */
  typedef Result (Class::*Method)(TP1);

public:
  StructuredMethodCallback_1P(Class* TheObject = NULL, typename StructuredMethodCallback_1P<Class, TP1, Result>::Method TheMethod = NULL);
  
  virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);
  
protected:
  Method TheMethod;
};

template <class Class, typename TP1>
  class StructuredMethodCallback_1P_V : public StructuredMethodCallback<Class>
{
protected:
  /** \brief Define the type of a method callback */
  typedef void (Class::*Method)(TP1);

public:
  StructuredMethodCallback_1P_V(Class* TheObject = NULL, typename StructuredMethodCallback_1P_V<Class, TP1>::Method TheMethod = NULL);
  
  virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);
  
protected:
  Method TheMethod;
};

template <class Class, typename TP1, typename TP2, typename Result>
  class StructuredMethodCallback_2P : public StructuredMethodCallback<Class>
{
protected:
  /** \brief Define the type of a method callback */
  typedef Result (Class::*Method)(TP1, TP2);

public:
  StructuredMethodCallback_2P(Class* TheObject = NULL, typename StructuredMethodCallback_2P<Class, TP1, TP2, Result>::Method TheMethod = NULL);
  
  virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);
  
protected:
  Method TheMethod;
};

template <class Class, typename TP1, typename TP2>
  class StructuredMethodCallback_2P_V : public StructuredMethodCallback<Class>
{
protected:
  /** \brief Define the type of a method callback */
  typedef void (Class::*Method)(TP1, TP2);

public:
  StructuredMethodCallback_2P_V(Class* TheObject = NULL, typename StructuredMethodCallback_2P_V<Class, TP1, TP2>::Method TheMethod = NULL);
  
  virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);
  
protected:
  Method TheMethod;
};

template <class Class, typename TP1, typename TP2, typename TP3, typename Result>
  class StructuredMethodCallback_3P : public StructuredMethodCallback<Class>
{
  protected:
    /** \brief Define the type of a method callback */
    typedef Result (Class::*Method)(TP1, TP2, TP3);

  public:
    StructuredMethodCallback_3P(Class* TheObject = NULL, typename StructuredMethodCallback_3P<Class, TP1, TP2, TP3, Result>::Method TheMethod = NULL);

    virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);

  protected:
    Method TheMethod;
};

template <class Class, typename TP1, typename TP2, typename TP3>
  class StructuredMethodCallback_3P_V : public StructuredMethodCallback<Class>
{
  protected:
    /** \brief Define the type of a method callback */
    typedef void (Class::*Method)(TP1, TP2, TP3);

  public:
    StructuredMethodCallback_3P_V(Class* TheObject = NULL, typename StructuredMethodCallback_3P_V<Class, TP1, TP2, TP3>::Method TheMethod = NULL);

    virtual bool Call(const StructuredParameters& Param, StructuredResult& Res);

  protected:
    Method TheMethod;
};

/* Implementation */

/* StructuredMethodCallback_0P */
template <class Class, typename Result>
  StructuredMethodCallback_0P<Class, Result>::StructuredMethodCallback_0P(Class* TheObject, typename StructuredMethodCallback_0P<Class, Result>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename Result>
  bool StructuredMethodCallback_0P<Class, Result>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    Res.SetResult( (this->TheObject->*(this->TheMethod))() );
    return true;
  } else {
    return false;
  }
}

/* StructuredMethodCallback_1P */
template <class Class, typename TP1, typename Result>
  StructuredMethodCallback_1P<Class, TP1, Result>::StructuredMethodCallback_1P(Class* TheObject, typename StructuredMethodCallback_1P<Class, TP1, Result>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1, typename Result>
  bool StructuredMethodCallback_1P<Class, TP1, Result>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    Param.GetParameter( 0, p1 );
    Res.SetResult( (this->TheObject->*(this->TheMethod))(p1) );
    return true;
  } else {
    return false;
  }
}

/* StructuredMethodCallback_1P_V */
template <class Class, typename TP1>
  StructuredMethodCallback_1P_V<Class, TP1>::StructuredMethodCallback_1P_V(Class* TheObject, typename StructuredMethodCallback_1P_V<Class, TP1>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1>
  bool StructuredMethodCallback_1P_V<Class, TP1>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    Param.GetParameter( 0, p1 );
    (this->TheObject->*TheMethod)(p1);
    /*Res.SetResult( true );*/
    return true;
  } else {
    return false;
  }
}


/* StructuredMethodCallback_2P */
template <class Class, typename TP1, typename TP2, typename Result>
  StructuredMethodCallback_2P<Class, TP1, TP2, Result>::StructuredMethodCallback_2P(Class* TheObject, typename StructuredMethodCallback_2P<Class, TP1, TP2, Result>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1, typename TP2, typename Result>
  bool StructuredMethodCallback_2P<Class, TP1, TP2, Result>::Call(const StructuredParameters& Param, StructuredResult& Res)
{  
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    TP2 p2;
    Param.GetParameter( 0, p1 );
    Param.GetParameter( 1, p2 );
    Res.SetResult( (this->TheObject->*(this->TheMethod))(p1, p2) );
    
    return true;
  } else {
    return false;
  }
}

/* StructuredMethodCallback_2P_V */
template <class Class, typename TP1, typename TP2>
  StructuredMethodCallback_2P_V<Class, TP1, TP2>::StructuredMethodCallback_2P_V(Class* TheObject, typename StructuredMethodCallback_2P_V<Class, TP1, TP2>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1, typename TP2>
  bool StructuredMethodCallback_2P_V<Class, TP1, TP2>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    TP2 p2;
    Param.GetParameter( 0, p1 );
    Param.GetParameter( 1, p2 );
    (this->TheObject->*TheMethod)(p1, p2);
    /*Res.SetResult( true );*/
    return true;
  } else {
    return false;
  }
}

/* StructuredMethodCallback_3P */
template <class Class, typename TP1, typename TP2, typename TP3, typename Result>
  StructuredMethodCallback_3P<Class, TP1, TP2, TP3, Result>::StructuredMethodCallback_3P(Class* TheObject, typename StructuredMethodCallback_3P<Class, TP1, TP2, TP3, Result>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1, typename TP2, typename TP3, typename Result>
  bool StructuredMethodCallback_3P<Class, TP1, TP2, TP3, Result>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    TP2 p2;
    TP3 p3;
    Param.GetParameter( 0, p1 );
    Param.GetParameter( 1, p2 );
    Param.GetParameter( 2, p3 );

    Res.SetResult( (this->TheObject->*(this->TheMethod))(p1, p2, p3) );

    return true;
  } else {
    return false;
  }
}

/* StructuredMethodCallback_3P_V */
template <class Class, typename TP1, typename TP2, typename TP3>
  StructuredMethodCallback_3P_V<Class, TP1, TP2, TP3>::StructuredMethodCallback_3P_V(Class* TheObject, typename StructuredMethodCallback_3P_V<Class, TP1, TP2, TP3>::Method TheMethod)
    : StructuredMethodCallback<Class>(TheObject, NULL)
{
  this->TheMethod = TheMethod;
}

template <class Class, typename TP1, typename TP2, typename TP3>
  bool StructuredMethodCallback_3P_V<Class, TP1, TP2, TP3>::Call(const StructuredParameters& Param, StructuredResult& Res)
{
  if(this->TheObject && TheMethod)
  {
    TP1 p1;
    TP2 p2;
    TP3 p3;
    Param.GetParameter( 0, p1 );
    Param.GetParameter( 1, p2 );
    Param.GetParameter( 2, p3 );
    (this->TheObject->*TheMethod)(p1, p2, p3);
    /*Res.SetResult( true );*/
    return true;
  } else {
    return false;
  }
}

}} // Omiscid::Messaging

#endif // __STRUCTURED_METHOD_CALL_BACK_NP__
