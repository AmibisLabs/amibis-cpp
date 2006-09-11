/* @file XsdValidator.h
 * @ingroup ServiceControl
 * @brief Header of the XsdValidator definition.
 * @date 2004-2006
 */

#ifndef __XSD_VALIDATOR_H__
#define __XSD_VALIDATOR_H__

#include <ServiceControl/Config.h>
#include <System/SimpleString.h>

#include <libxml/xmlschemas.h>

namespace Omiscid {

class XsdValidator
{
public:
	XsdValidator();
	virtual ~XsdValidator();

	bool CreateSchemaFromString( const SimpleString XsdSchemaToValidate );
	bool CreateSchemaFromFile( const SimpleString XsdSchemaFile );

	bool ValidateDoc( const SimpleString Doc );
	bool ValidateDoc( xmlDocPtr Doc );

private:
	void DestroySchema();

	xmlSchemaPtr Schema;
	xmlSchemaParserCtxtPtr ParserCtxt;
	xmlSchemaValidCtxtPtr ValidCtxt;
};

} // namespace Omiscid

#endif // __XSD_VALIDATOR_H__
