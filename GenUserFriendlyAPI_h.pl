require RecurseWork;

sub EnterDirectory()
{
 	my $DirName = shift @_;
 	my $UserData = shift @_;
 
 	if ( $DirName =~ /\.svn/ )
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
 	
 	if ( $FileName =~ /\/UserFriendlyAPI\.h/ || $FileName =~ /\/Config[^\/\.]+\.h/ )
 	{
 		return;
 	}
 	
	if ( $FileName =~ /\.h$/ )
	{
		if ( $UserData == 1 )	# All folder
		{
			$FileName =~ s/^[^\/]+\///;;
			push @ListOfFiles, $FileName;
			return;
		}
		open( $fd, "<$FileName" );
		while( $CurrentLine = <$fd> )
		{
			if ( $CurrentLine =~ /\@ingroup\s+UserFriendly[\s\r\n]/ )
			{
				close( $fd );
				$FileName =~ s/^[^\/]+\///;;
				push @ListOfFiles, $FileName;
				return;
			}
		}
		close( $fd );
	}
}

sub GenerateFile()
{
 	my $FileFolder = shift @_;	
 	my $FileName = shift @_;
 	my $ComputedHeader;
	my $fd;
	my $dummy, $annee;
	my $CurFile, $CurrentFolder;
	my $Tmp;
	
	$ComputedHeader = $FileName;
 	$ComputedHeader =~ s/([A-Z]+)/_$1/g;
 	$ComputedHeader =~ s/.h$/_H__/;
 	$ComputedHeader = '_' . $ComputedHeader;
 	$ComputedHeader =~ tr/a-z/A-Z/;
 	
	$FileName = "$FileFolder/$FileName";
	$FileName =~ s/^\/\//\//;
	$FileName =~ s/^\///;
 	
 	open( $fd, ">$FileName" ) or die "$FileName";
 	
 	print $fd "/* \@file $FileName\n * \@ingroup UserFriendly\n\n";
 	print $fd " * \@brief Files included when using user friendly API\n";
 	
 	my ( $dummy, $dummy, $dummy, $dummy, $dummy, $annee, $dummy, $dummy, $dummy ) = localtime( time );
 	$annee += 1900;
 	
 	print $fd " * \@date 2004-$annee\n * \@author Dominique Vaufreydaz\n */\n\n";
 	
 	print $fd "#ifndef $ComputedHeader\n#define $ComputedHeader\n";
 	
 	$CurrentFolder = '';
 	
 	foreach $CurFile ( @ListOfFiles )
 	{
 		$CurFile =~ /^([^\/]+)/;
 		$Tmp = $1;
 		if ( $Tmp ne $CurrentFolder )
 		{
 			$CurrentFolder= $Tmp;
 			
 			print $fd "\n// $CurrentFolder subfolder\n";
 		}
 		print $fd "#include <$CurFile>\n";
 	}
	
	print $fd "\n\n#endif // $ComputedHeader\n\n\n";
 	
 	close( $fd );
}

&RecurseWork::RecurseWork( 'System/System', 1 ); # 1 mean everything
&RecurseWork::RecurseWork( 'Com/Com', 0 );
&RecurseWork::RecurseWork( 'ServiceControl/ServiceControl/', 0 );
&RecurseWork::RecurseWork( 'Messaging/Messaging/', 1 );

&GenerateFile( 'ServiceControl/ServiceControl/', 'UserFriendlyAPI.h' );

@ListOfFiles =();

&RecurseWork::RecurseWork( 'System/System', 1 ); # 1 mean everything
&RecurseWork::RecurseWork( 'Com/Com', 1 );
&RecurseWork::RecurseWork( 'ServiceControl/ServiceControl/', 1 );
&RecurseWork::RecurseWork( 'Messaging/Messaging/', 1 );

&GenerateFile( '', 'Omiscid.h' );