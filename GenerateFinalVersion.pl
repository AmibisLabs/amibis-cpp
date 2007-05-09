use IPC::Open3;
use Cwd;

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

# default values for parameters
$IncrVersionType = 0;	# last
$DoTest = 1;

$PosArgv = 0;
while( defined $ARGV[$PosArgv] )
{
	if ( $ARGV[$PosArgv] eq '-middle' )
	{
		$IncrVersionType = 1; # middle number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] eq '-major' )
	{
		$IncrVersionType = 2; # major number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] eq '-minor' )
	{
		$IncrVersionType = 0; # major number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] eq '-notest' )
	{
		$DoTest = 0; # major number
		print "do not test\n";
		$PosArgv++;
		next;
	}
	die "Wrong parameter\n";
}

@UsualFiles = ( 'SConstruct', 'OmiscidScons.py', 'OmiscidInit.py', 'LICENCE', 'README', 'CHANGES', 'Doxyfile' );
$Version = "1.0.0";

$WorkingRep = cwd;

$WorkingRep =~ /\/([^\/]+)\/?$/;
$WorkingRep = $1;

if ( $WorkingRep eq '' )
{
	die "Unable to find working rep\n";
}

# $WorkingRep = 'OMiSCID-Dev'; # 'OMiSCID';

if ( -e '../LastVersion.info' )
{
	open( $fd, '<../LastVersion.info' ) or die "Unable to open '../LastVersion.info'\n";
	$line = <$fd>;
	if ( $line =~ /^(\d+)\.(\d+)\.(\d+)[\s\r\n]+$/ )
	{
		$majorn = $1;
		$middlen = $2;
		$lastn = $3;
		if ( $IncrVersionType == 0 )
		{
			$lastn++;
		}
		if ( $IncrVersionType == 1 )
		{
			$middlen++;
			$lastn = 0;
		}
		if ( $IncrVersionType == 2 )
		{
			$majorn++;
			$middlen = 0;
			$lastn = 0;
		}

		$Version = "$majorn.$middlen.$lastn";
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
	$command .= "$WorkingRep/$file ";
}
foreach $file ( keys %FilesToAdd )
{
	$command .= "$WorkingRep/$file ";
}

# print $command;

chdir('..');
system( $command );
chdir($WorkingRep);

if ( $DoTest == 1 )
{
	# $Computers{'astree'} = '000e0c5e4586';
	# $Options{'astree'}   = ' zeroconf=avahi ';
	# $Computers{'metis'}  = '000d936fc38c';
	# $Options{'metis'}   = '';
	$Computers{'junon'}  = '0013202e4fea';
	$Options{'junon'}   = ' zeroconf=mdns ';
	# $Computers{'desdemona'}  = '000bcd624fa9';
	# $Options{'desdemona'}   = ' zeroconf=mdns ';
	
	$TestsList{'RegisterSearchTest.cpp RegisterThread.cpp'} = 'RegisterTest';
	$TestsList{'BrowsingTest.cpp RegisterThread.cpp'} = 'BrowsingTest';
	
	$PutInEnv = "setenv LD_LIBRARY_PATH /tmp/OmiscidInstall/lib/:\$LD_LIBRARY_PATH;setenv DYLD_LIBRARY_PATH /tmp/OmiscidInstall/lib/:\$LD_LIBRARY_PATH;setenv PATH /tmp/OmiscidInstall/bin:\${PATH}:/tmp/OmiscidInstall/bin/";
	
	$NumComputer = 0;
	
	local (*READ, *WRITE, *ERROR);
	
	foreach $TestComputer ( keys %Computers )
	{
		# Check if computer if available
		print STDERR "Trying to connect to $TestComputer.\n";
		print STDERR "Send an etherwake command to $TestComputer.\n";
		$MacAddress = $Computers{$TestComputer};
		$MacAddress =~ s/://g;
		`./wol $MacAddress`;
		$NbTry = 10;
		while( $NbTry > 0 )
		{
			$res = `ssh $TestComputer "echo 'ssh is not ok.'"`;
			if ( $res =~ /^ssh is not ok\./ )
			{
				last;
			}
			print STDERR "Send an etherwake command to $TestComputer.\n";
			`./wol $MacAddress`;
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
		
		print STDERR "Try to install Omiscid.\n";
		open($ExecCommand, "ssh $TestComputer \"cd /tmp;rm -rf $WorkingRep OmiscidInstall;unzip $VersionFile;mkdir OmiscidInstall;cd $WorkingRep;scons debug=1 trace=1 $Options{$TestComputer} install prefix=/tmp/OmiscidInstall\" |");
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
		
		if ( $Tested == 0 )
		{
			last;
		}
		
		# set that it is not tested successfully
		$Tested = 0;
		
		print STDERR "Try to compile and run test(s).\n";
		foreach $test ( keys %TestsList )
		{
			# try to compile Test Examples
			`ssh $TestComputer "cd /tmp/$WorkingRep/Examples;rm -f ./Test;setenv LD_LIBRARY_PATH /tmp/OmiscidInstall/lib/:\$LD_LIBRARY_PATH;setenv PATH /tmp/OmiscidInstall/bin:\${PATH}:/tmp/OmiscidInstall/bin/;g++ -o Test $test \`OmiscidControl-config --all\``;
			$res = `ssh $TestComputer "${PutInEnv};cd /tmp/OmiscidInstall/lib/;/tmp/$WorkingRep/Examples/Test"`;
			if ( $res =~ /Test ok\./m )
			{
				# test is ok
				$Tested = 1;
			}
		}	
		
		# cleaning up
		`ssh $TestComputer "cd /tmp/$WorkingRep/;scons debug=1 trace=1 $Options{$TestComputer} install prefix=/tmp/OmiscidInstall -c;cd /tmp;rm -rf $WorkingRep OmiscidInstall $VersionFile"`;
		
		if ( $Tested == 0 )
		{
			last;
		}
	}
}
else
{
	$Tested = 1;
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
