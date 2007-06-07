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

sub CheckCommand()
{
	my $commandline = shift @_;
	my $checkline = shift @_;
	my $ExecCommand;
	my $ligne;
	
	open($ExecCommand, "$commandline |");
	while( $ligne = <$ExecCommand> )
	{
		print $ligne;
		if ( $ligne =~ /^$checkline/ )
		{
			# test is ok
			close( $ExecCommand );
			return 1;
		}
	}
	close( $ExecCommand );
	return 0;
}

$NameOfLogFile = 'Problems.txt';

sub EmptyLog()
{
	my $fdpb;
	# Empty problem file.
	open( $fdpb, ">$NameOfLogFile" );
	close( $fdpb );	
}

sub AddLog()
{
	my $ToAdd = shift@_;
	my $fdpb;
	
	# Empty problem file.
	open( $fdpb, ">>$NameOfLogFile" );
	print $fdpb "$ToAdd\n";
	close( $fdpb );	
}

sub LogOk()
{
	my $fdpb;
	my $line;
	
	# Empty problem file.
	open( $fdpb, "<$NameOfLogFile" );
	while( $line = <$fdpb> )
	{
		if ( $line =~ /^ERR:/ )
		{
			close( $fdpb );	
			return 0;
		}
	}
	close( $fdpb );	
	return 1;
}

# default values for parameters
$IncrVersionType = 0;	# last
$DoTest = 1;
$DoDoc = 1;
$ZipAfterZip = 0;

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
	if ( $ARGV[$PosArgv] eq '-stopafterfirstzip' )
	{
		$ZipAfterZip = 1; # major number
		print "just generate zip\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] eq '-nodoc' )
	{
		$DoDoc = 0; # major number
		print "do not generate documentation\n";
		$PosArgv++;
		next;
	}	
	die "Wrong parameter\n";
}

@UsualFiles = ( 'SConstruct', 'OmiscidScons.py', 'OmiscidInit.py', 'LICENCE', 'README', 'CHANGES', 'Doxyfile', 'TESTS' );
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


# generate testing archive
&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Examples/");
&RecurseWork::RecurseWork("Test/");

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

if ( $ZipAfterZip == 1 )
{
	exit();
}

&EmptyLog();

$TestSuite = "########################################\n#\n# report of automatic test procedure\n#\n########################################\n\n";

if ( $DoTest == 1 )
{
	# $Computers{'astree'} = '000e0c5e4586';
	# $Options{'astree'}   = '("zeroconf=avahi")';
	# $Computers{'metis'}  = '000d936fc38c';
	# $Options{'metis'}   = '("")';
	# $Computers{'junon'}  = '0013202e4fea';
	# $Options{'junon'}   = '("zeroconf=mdns")';
	# $Computers{'desdemona'}  = '000bcd624fa9';
	# $Options{'desdemona'}   = '("zeroconf=mdns")';
	# $Options{'desdemona'}   = '("")';
	$Computers{'deimos'}  = '000d561ff276';
	$Options{'deimos'}   = '("zeroconf=avahi")';

	
	$TestsList{'RegisterSearchTest.cpp RegisterThread.cpp'} = 'RegisterTest';
	$TestsList{'BrowsingTest.cpp RegisterThread.cpp'} = 'BrowsingTest';
	
	# $PutInEnv = "setenv LD_LIBRARY_PATH /tmp/OmiscidInstall/lib/:\$LD_LIBRARY_PATH;setenv DYLD_LIBRARY_PATH /tmp/OmiscidInstall/lib/:\$LD_LIBRARY_PATH;setenv PATH /tmp/OmiscidInstall/bin:\${PATH}:/tmp/OmiscidInstall/bin/";
	
	$NumComputer = 0;
	
	local (*READ, *WRITE, *ERROR);
	
	$NumComputer = 0;
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
			&AddLog( "ERR: Could not connect to $TestComputer" );
		}
		
		# incr num of computer 1
		$NumComputer++;
		
		$TestSuite .= "##################\n# Computer $NumComputer\n##################\nCMT: Compilation environment\n";
		
		# retrieve test suite
		print STDERR "Retrieve g++ version.\n";
		$gccversion = `ssh $TestComputer "g++ --version | head -n 1"`;
		$TestSuite .= "G++: $gccversion";
		
		print STDERR "Retrieve scons version.\n";
		$sconsversion = `ssh $TestComputer "scons --version"`;
		$sconsversion =~ s/[\r\n]+/ù/g;
		$sconsversion =~ s/^([^ù]+)ù//;
		$sconsversion =~ s/ù([^ù]+)ù$//;
		$sconsversion =~ s/ù/\n---: /;
		$sconsversion = "---: $sconsversion\n";
		$TestSuite .= "SCO: scons version :\n$sconsversion";
		
		print STDERR "Retrieve libxml version.\n";
		$xmlversion = `ssh $TestComputer "xml2-config --version"`;
		$TestSuite .= "XML: libxml2 version $xmlversion";
		
		# cleaning up everything
		print STDERR "Cleanup $TestComputer:/tmp/.\n";
		`ssh $TestComputer "cd /tmp/;rm -rf $WorkingRep OmiscidInstall $VersionFile"`;	
	        
		# copy Archive to the test computer
		print STDERR "Copy and unzip Omiscid on $TestComputer:/tmp/.\n";
		`scp ../$VersionFile $TestComputer:/tmp/;ssh $TestComputer "cd /tmp;rm -rf $WorkingRep OmiscidInstall;unzip $VersionFile;mkdir OmiscidInstall"`;
		
		print STDERR "Change permissions on test scripts on $TestComputer:/tmp/$WorkingRep/.\n";
		`ssh $TestComputer "chmod 755 /tmp/$WorkingRep/Test"`;

		# set that it is not tested successfully
		$Tested = 0;
		
		foreach $DebugFlag ( ('0', 'valgrind', '1') )
		{
			foreach $TraceFlag ( ('0', '1') )
			{
				$OptionString = "\@OptionsTable = $Options{$TestComputer};";
				eval( $OptionString );

				foreach $Option ( @OptionsTable )
				{
					print STDERR "Try to install Omiscid with 'debug=$DebugFlag trace=$TraceFlag $Option' flags.\n";
					$LastTry = "ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileOmiscid.sh $WorkingRep 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=/tmp/OmiscidInstall'\""; 
					$Tested = &CheckCommand( "ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileOmiscid.sh $WorkingRep 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=/tmp/OmiscidInstall'\"", 'scons: done building targets.' );
					if ( $Tested == 0 )
					{
						&AddLog( "ERR: Could not compile Omiscid on $TestComputer" );
						&AddLog( "CMT: $LastTry" );
						$TestSuite .= "ERR: Could not compile and install Omiscid with 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=/tmp/OmiscidInstall' flags\n";
						next;
					}
					else
					{
						$TestSuite .= "IOK: compile and install Omiscid\n---: flags used 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=/tmp/OmiscidInstall'\n";
					}					
					# set that it is not tested successfully
					$Tested = 0;
			
					print STDERR "Try to compile and run test(s) with 'debug=$DebugFlag trace=$TraceFlag $Option'.\n";
					foreach $test ( keys %TestsList )
					{
						# try to compile Test Examples
						$LastTry = "ssh $TestComputer $Options{$TestComputer} : \"/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test'\"";
						$Tested = 0;
						$NbTestsTried = 0;
						while( $NbTestsTried < 10 )
						{
							$res = `ssh $TestComputer "/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test'"`;
							if ( $res =~ /Test ok\./m )
							{
								# test is ok
								$Tested = 1;
								$TestSuite .= "TOK: compile TestsList{$test} and run it successfully\n---: files used '$test'\n";
								last;
							}
							sleep( 5 );
							$NbTestsTried++;
						}						
						if ( $Tested == 0 )
						{
							$TestSuite .= "TFA: Could not compile TestsList{$test} and run it successfully\n---: files used '$test'\n";
							if ( $Option =~ /avahi/ )
							{
								&AddLog( "WRN: Could not run test ('debug=$DebugFlag trace=$TraceFlag $Option') on $TestComputer" );
							}
							else
							{
								&AddLog( "ERR: Could not run test ('debug=$DebugFlag trace=$TraceFlag $Option') on $TestComputer" );
							}
							&AddLog( "CMT: $LastTry" );
							next;
						}
					}
					
					# cleaning up partially
					`ssh $TestComputer "rm -rf /tmp/OmiscidInstall"`;
				}
			}
		}
		# cleaning up everything
		print STDERR "Cleanup $TestComputer:/tmp/.\n";
		`ssh $TestComputer "cd /tmp/;rm -rf $WorkingRep OmiscidInstall $VersionFile"`;
		
		$TestSuite .= "\n";
	}
}
else
{
	$TestSuite .= "No test done.\n";
}

`rm ../$VersionFile`;
if ( &LogOk() == 0 )
{
	print STDERR "\n\n\t=> Problem when testing $VersionFile.\n";
	exit();
}


print STDERR "\n\n\t=> $VersionFile successfully generated.\n";
`echo $Version > ../LastVersion.info`;
print STDERR "Generate final version of $VersionFile.\n";

# generate report of tests
open( $fdtests, '>TESTS' );
print $fdtests $TestSuite;
close( $fdtests);

# reset file list
undef %FilesToAdd;

if ( $DoDoc == 1 )
{
	
	if ( -e 'Doc' && -d 'Doc' )
	{
		print "Remove Doc\n";
		`rm -rf Doc`;
	}
	
	print "Generate Doc\n";
	`doxygen`;
}

`perl RecursiveDos2Unix.pl`;

# generate testing archive
&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Examples/");
if ( $DoDoc == 1 )
{
	&RecurseWork::RecurseWork("Doc/");
}

print STDERR "Remove BipService.cpp from list.\n";
undef $FilesToAdd{'Examples/BipService.cpp'};

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
print STDERR "=> done.\n";
