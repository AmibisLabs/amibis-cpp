#include <ServiceControl/Attribute.h>

using namespace Omiscid;

Attribute::Attribute()
{}

Attribute::Attribute(const SimpleString a_name)
  : name(a_name)
{}

Attribute::~Attribute()
{}

void Attribute::AddTagDescriptionToStr(SimpleString& str)
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
      PutAValueInCData(formatDescription, str);
      str += "</formatDescription>";
    }
}

void Attribute::PutAValueInCData(const SimpleString val, SimpleString& str){
  str += "<![CDATA[";
  str += val;
  str += "]]>";
}

const SimpleString& Attribute::GetName() const
{
	return name;
}

const SimpleString& Attribute::GetDescription() const
{
	return description;
}

void Attribute::SetName(const SimpleString str)
{ 
	name = str;
}

void Attribute::SetDescription(const SimpleString str)
{
	description = str;
}

void Attribute::GenerateHeaderDescription(const SimpleString& type,
						const SimpleString& name,
						SimpleString& str,
						bool end)
{
  str = str + "<"+ type + " name=\"" + name;
  if(end) str = str + "\"/>";
  else  str = str + "\">";
}

const SimpleString& Attribute::GetFormatDescription() const
{
	return formatDescription;
}

void Attribute::SetFormatDescription(const SimpleString str)
{
	formatDescription = str;
}
