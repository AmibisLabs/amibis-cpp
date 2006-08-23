open( $fdh, '>ServiceControl/ServiceControl/XsdSchema.h' );
print $fdh qq{/* \@file XsdSchema.h
 * \@ingroup ServiceControl
 * \@brief Header of the Xsd Schema Strings definition. Automatically generated using GenerateXsdSupportFiles script.
 * \@date 2004-2006
 */
 
#ifndef __XSD_SCHEMA_H__
#define __XSD_SCHEMA_H__

#include <ServiceControl/Config.h>
#include <System/SimpleString.h>

namespace Omiscid \{


}; 

open( $fdinc, '>ServiceControl/XsdSchema.cpp' );
print $fdinc qq{/* \@file XsdSchema.cpp
 * \@ingroup ServiceControl
 * \@brief Body of the Xsd Schema Strings definition. Automatically generated using GenerateXsdSupportFiles script.
 * \@date 2004-2006
 */
#include <ServiceControl/XsdSchema.h>

using namespace Omiscid;

// Specific usage of string because of limitation of for constant string.
// This file is automatically generated using GenerateXsdSupportFiles.pl script.};

while( $file = <Xsd/*.xsd> )
{
	if ( $file =~ /\/([^\/\.]+).xsd$/ )
	{
		$name = $1;
		$fileinfo = "$name.xsd";
		
		# Generate extern definition in .h file
		$name =~ /^(.)(.+)$/;
		$name = uc($1) . $2;
		while( $name =~ /([^-]+)-(.)(.*)$/ )
		{
			$name = $1 . uc($2) . $3;
		}
		$name = $name . 'Xsd';
		print $fdh "extern const SimpleString ${name}Schema;\t// extern definition for $fileinfo verification\n";
		
		# Generate .inc content
		# print STDERR "Opening file '$file'\n";
		open( $fdin, "<$file" );
		
		print $fdinc "\nconst SimpleString Omiscid::${name}Schema = \"\"\n";
		while( $line = <$fdin> )
		{
			$line =~ s/[\s\r\n]+$//;
			
			if ( $line eq '' )
			{
				next;
			}
			
			$line =~ s/\"/\\\"/g;
			# $line .= " \\\n";
			
			print $fdinc "\"";
			print $fdinc $line;
			print $fdinc "\\n\"\n";
		}
		print $fdinc ";\n";
	}
}

print $fdinc "\n";
close( $fdinc );

print $fdh "\n} // namespace Omiscid\n\n#endif // __XSD_SCHEMA_H__\n"; 
close( $fdh );
