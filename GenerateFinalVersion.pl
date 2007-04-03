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

@UsualFiles = ( 'SConstruct', 'OmiscidScons.py', 'OmiscidInit.py', 'LICENCE', 'README', 'CHANGES', 'Doxyfile' );
$Version = "1.0.0";

if ( -e '../LastVersion.info' )
{
	open( $fd, '<../LastVersion.info' ) or die "Unable to open '../LastVersion.info'\n";
	$line = <$fd>;
	if ( $line =~ /^(\d+)\.(\d+)\.(\d+)[\s\r\n]+$/ )
	{
		$tmp = $3+1;
		$Version = "$1.$2.$tmp";
	}
	else
	{
		die "unable to parse '../LastVersion.info'\n";
	}
}

print "=> $Version\n";

$VersionFile = "Omiscid-$Version.zip";
if ( -e "../$VersionFile" )
{
	print "Remove $VersionFile\n";
	unlink "../$VersionFile";
}

if ( -e 'Doc' && -d 'Doc' )
{
	print "Remove Doc\n";
	`rm -rf Doc`;
}

print "Generate Doc\n";
`doxygen`;

`perl RecursiveDos2Unix.pl`;

&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Examples/");
&RecurseWork::RecurseWork("Doc/");

$command = "zip -9 $VersionFile ";
foreach $file ( @UsualFiles )
{
	$command .= "OMiSCID/$file ";
}
foreach $file ( keys %FilesToAdd )
{
	$command .= "OMiSCID/$file ";
}

# print $command;

chdir('..');
system( $command );
chdir('OMiSCID');

$Computers{'astree'} = '000e0c5e4586';
$Computers{'metis'}  = '000d936fc38c';
$Computers{'desdemona'}  = '000bcd624fa9';
$NumComputer = 0;

foreach $TestComputer ( keys %Computers )
{
	# Check if computer if available
	print STDERR "Trying to connect to $TestComputer.\n";
	$NbTry = 10;
	while( $NbTry > 0 )
	{
		$res = `ssh $TestComputer "echo 'ssh is not ok.'"`;
		if ( $res =~ /^ssh is not ok\./ )
		{
			last;
		}
		print STDERR "Send an etherwake command to $TestComputer.\n";
		$MacAddress = $Computers{$TestComputer};
		$MacAddress =~ s/://g;
		`wol $MacAddress`;
		sleep( 10 );
		$NbTry--;
	}
	if ( $NbTry == 0 )
	{
		$Tested = 0;
		last;
	}

	# copy Archive to the test computer
	`scp ../$VersionFile $TestComputer:/tmp/`;
	
	# set that it is not tested successfully
	$Tested = 0;
	
	open($ExecCommand, "ssh $TestComputer \"cd /tmp;rm -rf OMiSCID;unzip $VersionFile;cd OMiSCID;scons debug=1 trace=1\" |");
	while( $ligne = <$ExecCommand> )
	{
		print $ligne;
		if ( $ligne =~ /^scons: done building targets./ )
		{
			# test is ok
			$Tested = 1;
		}
	}
	close( $ExecCommand );
	
	# cleaning up
	`ssh $TestComputer "cd /tmp;rm -rf OMiSCID;rm -f $VersionFile"`;
}

if ( $Tested == 1 )
{
	print "\n\n\t=> $VersionFile successfully generated.\n";
	`echo $Version > ../LastVersion.info`;
}
else
{
	print "Could not generate and test on $TestComputer $VersionFile.\n";
	# clean up
	`rm ../$VersionFile`;
}
