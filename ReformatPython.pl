while( $ligne = <STDIN> )
{
	$ligne =~ s/^ /\t/;
	while( $ligne =~ s/^(\t+) /$1\t/ )
	{
	}
	print $ligne;
}