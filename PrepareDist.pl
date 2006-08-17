open( $fdh, '>ServiceControl/ServiceControl/Xsd.h' );
print $fdh "/* \@file Xsd.h\n * \@ingroup ServiceControl\n * \@brief Header of the Xsd definition\n * \@date 2004-2006\n */\n\n#ifndef __XSD_H__\n#define __XSD_H__\n\n#include <ServiceControl/Config.h>\n#include <System/SimpleString.h>\n\nnamespace Omiscid {\n\n"; 

open( $fdinc, '>ServiceControl/ServiceControl/Xsd.inc' );
print $fdinc "// Specific usage of string because of limitation of 16 Kb for constant string under Windows\n";

while( $file = <Xsd/*.xsd> )
{
	if ( $file =~ /\/(control-[^\/\.]+).xsd/ )
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
		$name = 'Xsd' . $name;
		print $fdh "extern SimpleString $name;\t// extern definition for $fileinfo verification\n";
		
		# Generate .inc content
		# print STDERR "Opening file '$file'\n";
		open( $fdin, "<$file" );
		
		print $fdinc "\nSimpleString $name = \"\"\n";
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
			print $fdinc "\"\n";
		}
		print $fdinc ";\n";
	}
}

print $fdinc "\n";
close( $fdinc );

print $fdh "\n} // namespace Omiscid\n\n#endif __XSD_H__\n"; 
close( $fdh );
