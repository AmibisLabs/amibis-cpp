#include <ServiceControl/Attribute.h>

#include <System/Portage.h>

using namespace Omiscid;

Attribute::Attribute()
{}

Attribute::Attribute(const SimpleString a_name)
: name(a_name)
{
}

Attribute::~Attribute()
{
}

void Attribute::AddTagDescriptionToStr(SimpleString& str)
{
	if ( description !=  "" )
	{
		str += "<description>"
			+ description
			+ "</description>";
	}
}

void Attribute::PutAValueInCData(const SimpleString val, SimpleString& str)
{
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
	// "<"+ type + " name=\"" + name + "\"/>" at max
	TemporaryMemoryBuffer MemBuff( 1 + type.GetLength() + 7 + name.GetLength() + 4 );
	if ( end == true )
	{
		snprintf( (char*)MemBuff, MemBuff.GetLength(), "<%s name=\"%s\"/>", type.GetStr(), name.GetStr() );
	}
	else
	{
		snprintf( (char*)MemBuff, MemBuff.GetLength(), "<%s name=\"%s\">", type.GetStr(), name.GetStr() );
	}

	str += (char*)MemBuff;

	//str = str + "<"+ type + " name=\"" + name;
	//if(end) str = str + "\"/>";
	//else  str = str + "\">";
}

