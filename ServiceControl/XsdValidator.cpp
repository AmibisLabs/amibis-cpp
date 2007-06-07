/* @file XsdValidator.h
 * @ingroup ServiceControl
 * @brief Header of the XsdValidator definition.
 * @date 2004-2006
 */

#include <ServiceControl/XsdValidator.h>

using namespace Omiscid;

XsdValidator::XsdValidator()
{
    ParserCtxt = NULL;
    Schema = NULL;
    ValidCtxt = NULL;
}

XsdValidator::~XsdValidator()
{
    DestroySchema();
}

void XsdValidator::DestroySchema()
{
    // Cleaning xsd validation tools
    if ( ValidCtxt )
    {
        xmlSchemaFreeValidCtxt( ValidCtxt );
        ValidCtxt = NULL;
    }
    if ( Schema )
    {
        xmlSchemaFree( Schema );
        Schema = NULL;
    }
    if ( ParserCtxt )
    {
        xmlSchemaFreeParserCtxt( ParserCtxt );
        ParserCtxt = NULL;
    }
}

bool XsdValidator::CreateSchemaFromString( const SimpleString XsdSchemaToValidate /* = SimpleString::EmptyString */ )
{
    if ( ValidCtxt != NULL )
    {
        // Already create
        OmiscidError( "CreateSchemaFromString: Schema is already initialised. Call DestroySchema() first.\n" );
        return false;
    }

    if ( XsdSchemaToValidate == SimpleString::EmptyString )
    {
        OmiscidError( "CreateSchemaFromFile: Invalid parameter.\n" );
        return false;
    }

    // Construct xsd validation tools
    ParserCtxt    = xmlSchemaNewMemParserCtxt( XsdSchemaToValidate.GetStr(), XsdSchemaToValidate.GetLength() );
    if ( ParserCtxt == NULL )
    {
        OmiscidError( "CreateSchemaFromString: Invalid parameter.\n" );
        return false;
    }
    Schema = xmlSchemaParse( ParserCtxt );
    if ( Schema == NULL )
    {
        xmlSchemaFreeParserCtxt( ParserCtxt );
        OmiscidError( "CreateSchemaFromString: Invalid parameter.\n" );
        return false;
    }
    ValidCtxt = xmlSchemaNewValidCtxt( Schema );
    if ( ValidCtxt == NULL )
    {
        xmlSchemaFree( Schema );
        xmlSchemaFreeParserCtxt( ParserCtxt );
        OmiscidError( "CreateSchemaFromString: Invalid parameter.\n" );
        return false;
    }

#ifdef DEBUG
    // OmiscidTrace errors during validation
    xmlSchemaSetValidErrors( ValidCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
#endif

    return true;
}

bool XsdValidator::CreateSchemaFromFile( const SimpleString XsdSchemaFile )
{
    if ( ValidCtxt != NULL )
    {
        // Already create
        OmiscidError( "CreateSchemaFromFile: Schema is already initialised. Call DestroySchema() first.\n" );
        return false;
    }

    if ( XsdSchemaFile == SimpleString::EmptyString )
    {
        OmiscidError( "CreateSchemaFromFile: Invalid parameter.\n" );
        return false;
    }

    // Construct xsd validation tools
    ParserCtxt    = xmlSchemaNewParserCtxt( XsdSchemaFile.GetStr() );
    if ( ParserCtxt == NULL )
    {
        OmiscidError( "CreateSchemaFromFile: Invalid parameter.\n" );
        return false;
    }
    Schema = xmlSchemaParse( ParserCtxt );
    if ( Schema == NULL )
    {
        xmlSchemaFreeParserCtxt( ParserCtxt );
        OmiscidError( "CreateSchemaFromFile: Invalid parameter.\n" );
        return false;
    }
    ValidCtxt = xmlSchemaNewValidCtxt( Schema );
    if ( ValidCtxt == NULL )
    {
        xmlSchemaFree( Schema );
        xmlSchemaFreeParserCtxt( ParserCtxt );
        OmiscidError( "CreateSchemaFromFile: Invalid parameter.\n" );
        return false;
    }

#ifdef DEBUG
    // OmiscidTrace errors during validation
    xmlSchemaSetValidErrors( ValidCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
#endif

    return true;
}

bool XsdValidator::ValidateDoc( const SimpleString Doc )
{
    xmlDocPtr pDoc;
    bool ret;

    // First parse doc in order to get a tree
    if ( ValidCtxt == NULL || Doc.IsEmpty() || (pDoc=xmlParseMemory(Doc.GetStr(),Doc.GetLength())) == NULL )
    {
        OmiscidError( "ValidateDoc: unable to validate xml stream.\n" );
        return false;
    }

    // Call the valisation process over the pDoc;
    ret = ValidateDoc( pDoc );

    // Free the tree
    xmlFreeDoc( pDoc );

    // return parsing result
    return ret;
}

bool XsdValidator::ValidateDoc( xmlDocPtr Doc )
{
    if ( ValidCtxt == NULL || Doc == NULL || xmlSchemaValidateDoc( ValidCtxt, Doc ) != 0 )
    {
        OmiscidError( "ValidateDoc: unable to validate xml stream.\n" );
        return false;
    }

    return true;
}

































































