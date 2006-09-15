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

$Version = "1.0.0";

if ( -e 'LastVersion.info' )
{
	open( $fd, '<LastVersion.info' ) or die "Unable to open 'LastVersion.info'\n";
	$line = <$fd>;
	if ( $line =~ /^\d+\.\d+\.\d+[\s\r\n]+$/ )
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

&RecurseWork::RecurseWork("System/");

$command = "zip -9 Omiscid-$Version.zip ";
foreach $file ( keys %FilesToAdd )
{
	$command .= "$file ";
}

print $command;

system( $command );