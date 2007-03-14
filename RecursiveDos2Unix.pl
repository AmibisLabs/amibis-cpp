require RecurseWork;

sub Reformat()
{
	my $FileName = shift @_;
	my $fd;
	my $ligne;
	my $contenu = '';
	
	print STDERR "$FileName\n";
	
	open( $fd, "<$FileName" ) or die "Could not open '$FileName'\n";
	while( $ligne = <$fd> )
	{
		$ligne =~ s/[\r\n\s]+$//g;
		$ligne =~ s/\t/    /g;
		$contenu .= $ligne . "\n";
	}
	close( $fd );
	
	open( $fd, ">$FileName" ) or die "Could not write to '$FileName'\n";
	print $fd $contenu;
	print $fd "\n";
	close( $fd );	
}

sub WorkOnFile()
{
 	my $FileName = shift @_;
 	my $UserData = shift @_;

	if ( $FileName =~ /\.(cpp|h|txt|xml|xsd)/ )
	{
		&Reformat($FileName);
	}

	if ( $FileName =~ /^SConstruct|LICENCE|README|CHANGES|Doxyfile$/ )
	{
		&Reformat($FileName);
	}
}

sub EnterDirectory()
{
 	my $DirName = shift @_;
	my $UserData = shift @_;
	
	if ( $DirName =~ /\.svn/ || $DirName =~ /VisualStudio.in|WorkingForOMiSCID|Spec|Xsd/ )
	{
		return 0;
	}
		
	# print "$DirName\n";
 
 	return 1;
}

&RecurseWork::RecurseWork('.','');