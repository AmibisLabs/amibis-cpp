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
 	my $NumLine;

	if ( defined $NotParseThisFiles{$FileName} )
	{
		return
	}

	if ( $FileName =~ /\.(h|cpp)$/ )
	{
		$NumLine = 0;
		open( $fd, "<$FileName" ) or return;
		while( $CurrentLine = <$fd> )
		{
			$NumLine++;
			if ( $CurrentLine =~ /char\s+\*/ || $CurrentLine =~ /char\s+\w+(\s)?\[/)
			{
				$CurrentLine =~  s/[\r\n]+$//;
				print "$FileName [$NumLine]: $CurrentLine\n";
			}
		}
		close( $fd );
		
	}
}

@ExcludedFiles = (
'System/Socket.cpp',
'System/System/Config.h',
'System/System/Portage.h',
'System/System/SimpleException.h',
'System/System/SimpleString.h'
);

foreach $fic ( @ExcludedFiles )
{
	$NotParseThisFiles{$fic} = 1;
}

&RecurseWork::RecurseWork( 'System', 0 );
# &RecurseWork::RecurseWork( 'Com', 0 );
# &RecurseWork::RecurseWork( 'ServiceControl', 0 );