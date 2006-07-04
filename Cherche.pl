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
'System/System/Config.h',
'System/Socket.cpp',
'System/Portage.cpp',
'System/SimpleException.cpp',
'System/SimpleString.cpp',
'Com/MsgSocket.cpp',
'Com/TcpUdpClientServer.cpp',
'Com/Com/TcpServer.h'
);

foreach $fic ( @ExcludedFiles )
{
	if ( $fic =~ /\.cpp/ )
	{
		$NotParseThisFiles{$fic} = 1;
		# print STDERR "$fic\n";
		$fic =~ /([^\/]+)\/([^\/]+)\.cpp$/;
		$fic = "$1/$1/$2.h";
		$NotParseThisFiles{$fic} = 1;
		# print STDERR "$fic\n";
	}
	else
	{
		$NotParseThisFiles{$fic} = 1;
		# print STDERR "$fic\n";
	}
}

&RecurseWork::RecurseWork( 'System', 0 );
&RecurseWork::RecurseWork( 'Com', 0 );
# &RecurseWork::RecurseWork( 'ServiceControl', 0 );