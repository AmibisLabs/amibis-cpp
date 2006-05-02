#include <ServiceControl/Attribut.h>

Attribut::Attribut()
{}

Attribut::Attribut(const SimpleString& a_name)
  : name(a_name)
{}

Attribut::Attribut(const char* a_name)
  : name(a_name)
{}

Attribut::~Attribut()
{}

void Attribut::AddTagDescriptionToStr(SimpleString& str)
{
  if(description !=  "")
    {
      str = str 
	+ "<description>"
	+ description
	+ "</description>";   
    }

  if(formatDescription != "")
    {
      str += "<formatDescription>";
      PutAValueInCData(formatDescription.GetStr(), str);
      str += "</formatDescription>";
    }
}

void Attribut::PutAValueInCData(const char* val, SimpleString& str){
  str += "<![CDATA[";
  str += val;
  str += "]]>";
}
