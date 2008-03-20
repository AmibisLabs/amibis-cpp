sub TraiteRep()
{
	my $rep = shift @_;
	my @liste = <$rep/*>;
	my $element;
	
	# print STDERR "'$rep'\n";
	
	foreach $element ( @liste )
	{
		if ( -d $element )
		{
			next if $element =~ /^\.\.?$/;
			
			if ( $element =~ /\/CVS$/ )
			{
				# print STDERR "removing $element...\n";
				# `rm -rf $element`;
				next;
			}
			
			&TraiteRep( $element );
			next;
		}
		
		if ( $element =~ /(\.[^\.\/]+)$/ )
		{
			$extension = $1;
			# next if ( $extension =~ /^\.(h|c|cpp|cc|scm|pl|asm|java|vbs|bmp|rc|rc2|ico|txt|bat|hlp|howto|ps|hh|pdf|res|resources|tcl|zip)$/i );
			$Extensions{$1}++;
		}
	}
}

&TraiteRep( $ARGV[0] );

foreach $element ( sort keys %Extensions )
{
	print "*$element\n"; # $Extensions{$element}\n";
}
