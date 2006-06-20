#include <ServiceControl/Attribut.h>

using namespace Omiscid;

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

const SimpleString& Attribut::GetName() const
{
	return name;
}

const SimpleString& Attribut::GetDescription() const
{
	return description;
}

const char* Attribut::GetNameCh() const
{
	return name.GetStr();
}

const char* Attribut::GetDescriptionCh() const
{
	return description.GetStr();
}

void Attribut::SetName(const char* str)
{ 
	name = str;
}

void Attribut::SetDescription(const char* str)
{
	description = str;
}

void Attribut::SetName(const SimpleString& str)
{
	name = str;
}

void Attribut::SetDescription(const SimpleString& str)
{
	description = str;
}

void Attribut::GenerateHeaderDescription(const SimpleString& type,
						const SimpleString& name,
						SimpleString& str,
						bool end)
{
  str = str + "<"+ type + " name=\"" + name;
  if(end) str = str + "\"/>";
  else  str = str + "\">";
}

const SimpleString& Attribut::GetFormatDescription() const
{
	return formatDescription;
}

const char* Attribut::GetFormatDescriptionCh() const
{
	return formatDescription.GetStr();
}

void Attribut::SetFormatDescription(const SimpleString& str)
{
	formatDescription = str;
}

void Attribut::SetFormatDescription(const char* str)
{
	formatDescription = str;
}