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
			# if ( $CurrentLine =~ /char\s*\*/ || $CurrentLine =~ /char\s*\w+(\s)?\[/)
			if ( $CurrentLine =~ /SimpleString\s*\&/)
			{
				$CurrentLine =~  s/[\r\n]+$//;
				print "$FileName [$NumLine]: $CurrentLine\n";
			}
		}
		close( $fd );
		
	}
}

# @ExcludedFiles = (
# 'System/System/Config.h',
# 'System/Socket.cpp',
# 'System/Portage.cpp',
# 'System/SimpleException.cpp',
# 'System/SimpleString.cpp',
# 'Com/MsgSocket.cpp',
# 'Com/TcpUdpClientServer.cpp',
# 'Com/TcpServer.cpp',
# 'Com/UdpExchange.cpp',
# 'ServiceControl/ControlClient.cpp',
# 'Com/Message.cpp',
# 'Com/TcpClient.cpp',
# 'ServiceControl/BrowseForDnsSdService.cpp',
# 'ServiceControl/OmiscidService.cpp',
# 'ServiceControl/Service.cpp',
# 'ServiceControl/ServiceProperties.cpp',
# 'ServiceControl/WaitForDnsSdServices.cpp',
# 'ServiceControl/ServicesCommon.cpp',
# 'ServiceControl/ControlServer.cpp',
# 'ServiceControl/ControlUtils.cpp',
# 'ServiceControl/XMLTreeParser.cpp',
# 'ServiceControl/VariableAttribut.cpp',
# 'ServiceControl/InOutputAttribut.cpp',
# 'ServiceControl/ServiceFromXML.cpp'
# );

@ExcludedFiles = (
'System/SimpleString.cpp',
'Com/ComTools.cpp',
'ServiceControl/ControlServer.cpp',
'ServiceControl/Attribut.cpp',
'ServiceControl/ControlClient.cpp',
'ServiceControl/InOutputAttribut.cpp',
'ServiceControl/ControlUtils.cpp',
'ServiceControl/OmiscidServiceFilters.cpp',
'ServiceControl/OmiscidServiceProxy.cpp',
'ServiceControl/VariableAttribut.cpp'
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
&RecurseWork::RecurseWork( 'ServiceControl', 0 );