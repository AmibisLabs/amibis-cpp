require RecurseWork;

sub EnterDirectory()
{
 	my $DirName = shift @_;
 	my $UserData = shift @_;
 
 	if ( $DirName =~ /\.svn$/ )
 	{
 		return 0;
 	}
 	return 1;
}

sub WorkOnFile()
{
 	my $FileName = shift @_;
 	my $UserData = shift @_;
 	my $fd;
 	my $CurrentLine;
 	my $PreviousLineWasEndedByReturn;

	if ( $FileName =~ /\.(h|cpp)$/ )
	{
		$PreviousLineWasEndedByReturn = 0;
		open( $fd, "<$FileName" ) or return;
		while( $CurrentLine = <$fd> )
		{
			if ( $CurrentLine =~ /[\r\n]+$/ )
			{
				$PreviousLineWasEndedByReturn = 1; 
			}
			else
			{
				$PreviousLineWasEndedByReturn = 0; 
			}
		}
		close( $fd );
		
		if ( $PreviousLineWasEndedByReturn == 0 )
		{
			print STDERR "You should add an empty line at end of '$FileName' to be gcc compliant\n";
		}
	}
}

&RecurseWork::RecurseWork( '.', 0 );