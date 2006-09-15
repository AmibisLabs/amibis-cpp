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

	$FilesToAdd{$FileName} = 1;
}

@UsualFiles = ( 'SConstruct', 'OmiscidScons.py', 'LICENCE', 'README', 'Doxyfile' );
$Version = "1.0.0";

if ( -e 'LastVersion.info' )
{
	open( $fd, '<LastVersion.info' ) or die "Unable to open 'LastVersion.info'\n";
	$line = <$fd>;
	if ( $line =~ /^(\d+)\.(\d+)\.(\d+)[\s\r\n]+$/ )
	{
		$tmp = $3+1;
		$Version = "$1.$2.$tmp";
	}
	else
	{
		die "unable to parse 'LastVersion.info'\n";
	}
}

print "=> $Version\n";

$VersionFile = "Omiscid-$Version.zip";
if ( -e $VersionFile )
{
	print "Remove $VersionFile";
	`rm -rf $VersionFile`;
}

if ( -e 'Doc' && -d 'Doc' )
{
	print "Remove Doc";
	`rm -rf Doc`;
}

print "Generate Doc\n";
`doxygen`;

&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControle/");
&RecurseWork::RecurseWork("Examples/");
&RecurseWork::RecurseWork("Doc/");

$command = "zip -9 $VersionFile ";
foreach $file ( @UsualFiles )
{
	$command .= "OMiSCID-Dev/$file ";
}
foreach $file ( keys %FilesToAdd )
{
	$command .= "OMiSCID-Dev/$file ";
}

# print $command;

chdir('..');
system( $command );

`echo $Version > LastVersion.info`;