while( 1 )
{
	print "Entrez le type de variable : ";
	$Type = <STDIN>;
	$Type =~ s/[\r\n]+//g;
	print "Entrez le nom de la variable : ";
	$Nom = <STDIN>;
	$Nom =~ s/[\r\n]+//g;
	print "Entrez l'init de la variable : ";
	$InitVar = <STDIN>;
	$InitVar =~ s/[\r\n]+//g;
	print "inline static ${Type}& $Nom()\n{\n\tstatic $Type Internal_$Nom${InitVar};\n\treturn Internal_$Nom;\n}\n";
}