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

	print STDERR "Execute '$commandline'\n";
	open($ExecCommand, "$commandline |");
	while( $ligne = <$ExecCommand> )
	{
		print STDERR $ligne;
		if ( $ligne =~ /^$checkline/ )
		{
			# test is ok
			# close( $ExecCommand );
			return 1;
		}
	}
	# close( $ExecCommand );
	return 0;
}

sub WakeOnLan()
{
	my $TestComputer = shift @_;		
	my $MacAddress;
	my $res;
	
	if ( !defined $Computers{$TestComputer} )
	{
		return 0;
	}
	
	$MacAddress = $Computers{$TestComputer};
	
	$MacAddress =~ s/://g;
	`./wol $MacAddress`;
	$NbTry = 5;
	while( $NbTry > 0 )
	{
		$res = `ssh $TestComputer "echo 'ssh is now ok.'"`;
		if ( $res =~ /^ssh is now ok\./ )
		{
			return 1;
		}
		sleep( 5 );
		print STDERR "Send an etherwake command to $TestComputer.\n";
		`./wol $MacAddress`;
		$NbTry--;
	}
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
$StopAfterZip = 0;

$PosArgv = 0;
while( defined $ARGV[$PosArgv] )
{
	if ( $ARGV[$PosArgv] =~ /^-middle/i )
	{
		$IncrVersionType = 1; # middle number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-major/i )
	{
		$IncrVersionType = 2; # major number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-minor/i )
	{
		$IncrVersionType = 0; # minor number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-last/i )
	{
		$IncrVersionType = -1; # regenerate the same version
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-notest/i )
	{
		$DoTest = 0; # major number
		print "do not test\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-stopafterzip/i )
	{
		$StopAfterZip = 1; # major number
		print "just generate zip\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-nodoc/i )
	{
		$DoDoc = 0; # major number
		print "do not generate documentation\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-emptylog/i )
	{
		&EmptyLog();
		print "log is empty\n";
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
		if ( $IncrVersionType == -1 )
		{
			# Nothing
		}
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

&AddLog( "-----------------------------------------\n  Start test phase on $Version version\n-----------------------------------------" );
# die;

$VersionFile = "Omiscid-$Version.zip";
if ( -e "../$VersionFile" )
{
	print "Remove $VersionFile\n";
	unlink "../$VersionFile";
}

# generate temporary CHANGES from CHANGES.in
`cp CHANGES.in CHANGES`;

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

if ( $StopAfterZip == 1 )
{
	exit();
}

$TestSuite  = "###########################################################\n";
$TestSuite .= "#\n";
$TestSuite .= "# Report of automatic test procedure\n";
$TestSuite .= "#\n";
$TestSuite .= "###########################################################\n";
$TestSuite .= "# You will find a list of test computers, and\n";
$TestSuite .= "# for each one, you may have the following flags:\n";
$TestSuite .= "#    CMT: comments\n";
$TestSuite .= "#    CMD: commande lauched\n";
$TestSuite .= "#    C++: version of C++ compiler installed on test computer\n";
$TestSuite .= "#    SCO: scons version\n";
$TestSuite .= "#    XML: lib XML version\n";
$TestSuite .= "#    IOK: compilation and installation of Omiscid was ok\n";
$TestSuite .= "#    IFA: compilation and installation of Omiscid failed\n";
$TestSuite .= "#    TOK: the test is ok\n";
$TestSuite .= "#    TFA: the test failed\n";
$TestSuite .= "###########################################################\n";
$TestSuite .= "\n\n";
$TestSuite .= "##################\n";
$TestSuite .= "# Computer 1\n";
$TestSuite .= "##################\n";
$TestSuite .= "CMT: Compilation environment\n";
$TestSuite .= "C++: Microsoft Visual Studio 2005\n";
$TestSuite .= "     Version 8.0.50727.867  (vsvista.050727-8600)\n";
$TestSuite .= "     Microsoft .NET Framework\n";
$TestSuite .= "     Version 2.0.50727 SP1\n";
$TestSuite .= "     Édition installée : Professional\n";
$TestSuite .= "     Microsoft Visual C# 2005   77915-009-0000007-41342\n";
$TestSuite .= "     Microsoft Visual C++ 2005   77915-009-0000007-41342\n";
$TestSuite .= "     Microsoft Visual J# 2005   77915-009-0000007-41342\n";
$TestSuite .= "     Microsoft Web Application Projects 2005   77915-009-0000007-41342\n";
$TestSuite .= "     Version 8.0.50727.867\n";
$TestSuite .= "     Crystal Reports    AAC6G-G0CSA4K-U7000P7\n";
$TestSuite .= "     Microsoft Visual Studio 2005 Professional - Français Service Pack 1 (KB926607)\n";
$TestSuite .= "     Security Update pour Microsoft Visual Studio 2005 Professional - Français (KB937061)\n";
$TestSuite .= "     Update pour Microsoft Visual Studio 2005 Professional - Français (KB932233)\n";
$TestSuite .= "     Insure++ Version 7.1.2 (build 2007-09-10)\n";
$TestSuite .= "XML: libxml2 version 2.6.23 with iconv 1.9.1\n";

# Construct the actual tests done
foreach $DebugFlag ( ('1', '0') )
{
	if ( $DebugFlag eq '1' )
	{
		$DebugTag = '1 chmem=1 ';
	}
	else
	{
		$DebugTag = '0';
	}
	
	foreach $TraceFlag ( ('1', '0') )
	{
		$TestSuite .= "CMT: Test Omiscid with 'debug=$DebugTag trace=$TraceFlag' flags.\n";
		$TestSuite .= "IOK: Compile Omiscid.\n";
		$TestSuite .= "TOK: Compile GlobalTest and run it successfully (1)\n";
		$TestSuite .= "     files used 'GlobalTest.cpp Accumulator.cpp BrowsingTest.cpp ClientAccumulator.cpp RegisterSearchTest.cpp RegisterThread.cpp SendHugeData.cpp'\n";
	}
}

if ( $DoTest == 1 )
{
	$Computers{'astree'} = '000e0c5e4586';
	$Options{'astree'}   = '("zeroconf=avahi")'; # debugthread=1")';
	$Computers{'prometheus'} = '0013202e4fae';
	$Options{'prometheus'}   = '("")'; # debugthread=1")';
	$Computers{'metis'}  = '000d936fc38c';
	$Options{'metis'}   = '("")';
	$Computers{'desdemona'}  = '000bcd624fa9';
	# $Options{'desdemona'}   = '("zeroconf=mdns")';
	$Options{'desdemona'}   = '("")';
	$Computers{'protee'}  = '000d561ff276';
	$Options{'protee'}   = '("zeroconf=avahi ChMemMode=1", "")';
	# $Options{'protee'}   = '("zeroconf=mdns ChMemMode=1")';
	# $Options{'protee'}   = '("zeroconf=mdns chmem=1")';
	# $SupportedDebugMode{'protee'} = 'insure';
	$Computers{'puck'}  = '0019b94b4902';
	$Options{'puck'}   = '("")';
	
	$TestsList{'RegisterSearchTest.cpp RegisterThread.cpp'} = 'RegisterTest';
	$TestsList{'BrowsingTest.cpp RegisterThread.cpp'} = 'BrowsingTest';
	$TestsList{'SendHugeData.cpp'} = 'SendHugeData';
	
	$InstallOmiscidFolder = '/tmp/OmiscidInstall';
	# $InstallOmiscidFolder = '/usr/local/';
	
	# $PutInEnv = "setenv LD_LIBRARY_PATH $InstallOmiscidFolder/lib/:\$LD_LIBRARY_PATH;setenv DYLD_LIBRARY_PATH $InstallOmiscidFolder/lib/:\$LD_LIBRARY_PATH;setenv PATH $InstallOmiscidFolder/bin:\${PATH}:$InstallOmiscidFolder/bin/";
	
	$NumComputer = 0;
	
	local (*READ, *WRITE, *ERROR);
	
	$NumComputer = 1;
	foreach $TestComputer ( keys %Computers )
	{
		# Check if computer if available
		print STDERR "Trying to connect to $TestComputer.\n";
		&AddLog( "TRY: Trying to connect to $TestComputer." );
		print STDERR "Send an etherwake command to $TestComputer.\n";
		if ( &WakeOnLan($TestComputer) == 0 )
		{
			$Tested = 0;
			&AddLog( "WRN: Could not connect to $TestComputer" );
			next;
		}
		
		&AddLog( "TOK: Connected to $TestComputer." );
		
		# incr num of computer 1
		$NumComputer++;
		
		$TestSuite .= "\n\n##################\n# Computer $NumComputer\n##################\n\n";
		
		@DebugModesForTest = ('1', '0'); # 'valgrind',
		
		if ( defined $SupportedDebugMode{$TestComputer} )
		{
			unshift @DebugModesForTest, $SupportedDebugMode{$TestComputer};
		}

		foreach $DebugFlag ( @DebugModesForTest )
		{
			$TestSuite .= "CMT: Compilation environment\n";
			
			$Compiler = 'g++ ';
			$CompilerOptions = ' ';
			$InvoqueDebugger = ' ';
			
			# retrieve test suite
			if ( $DebugFlag =~ /^(1|yes|true|valgrind|insure)$/ )
			{				
				# We are in debug mode
				$CompilerOptions = ' -g ';
				
				if ( $DebugFlag =~ /insure/ )
				{
					print STDERR "$TestComputer: Retrieve insure version\n";
					$insureversion = `ssh $TestComputer "insure --version"`;
					$TestSuite .= "INS: $insureversion";
					$Compiler = 'insure ';
				}
				else
				{
					if ( $DebugFlag =~ /valgrind/ )
					{
						print STDERR "$TestComputer: Retrieve valgrind version\n";
						$valgrindversion = `ssh $TestComputer "valgrind --version"`;
						$TestSuite .= "VAL: $insureversion";
						
						# Compiler remain g++
						$CompilerOptions .= ' -O0 -fno-inline ';
						$InvoqueDebugger = ' valgrind --tool=memcheck -v ';
					}
				}
			}
			
			if ( $Options{$TestComputer} =~ /debugthread\=(1|yes|true)/ )
			{
				$CompilerOptions .= ' -DDEBUG_THREAD ';
			}
			
			if ( $Options{$TestComputer} =~ /chmem\=(1|yes|true)/ )
			{
				$CompilerOptions .= ' -DTRACKING_MEMORY_LEAKS ';
			}		
	
			print STDERR "$TestComputer: Retrieve g++ version\n";
			$gccversion = `ssh $TestComputer "g++ --version | head -n 1"`;
			$TestSuite .= "C++: $gccversion";
			
			print STDERR "$TestComputer: Retrieve scons version\n";
			$sconsversion = `ssh $TestComputer "scons --version"`;
			$sconsversion =~ s/[\r\n]+/ù/g;
			$sconsversion =~ s/^([^ù]+)ù//;
			$sconsversion =~ s/ù([^ù]+)ù$//;
			$sconsversion =~ s/ù/\n     /;
			$sconsversion = "     $sconsversion\n";
			$TestSuite .= "SCO: scons version :\n$sconsversion";
			
			print STDERR "$TestComputer: Retrieve libxml version\n";
			$xmlversion = `ssh $TestComputer "xml2-config --version"`;
			$TestSuite .= "XML: libxml2 version $xmlversion";
			
			# cleaning up everything
			print STDERR "$TestComputer: Cleanup /tmp/\n";
			`ssh $TestComputer "cd /tmp/;rm -rf $WorkingRep OmiscidInstall $VersionFile"`;	
		        
			# copy Archive to the test computer
			print STDERR "$TestComputer: Copy and unzip Omiscid in /tmp/\n";
			`scp ../$VersionFile $TestComputer:/tmp/;ssh $TestComputer "cd /tmp;rm -rf $WorkingRep OmiscidInstall;unzip $VersionFile;mkdir OmiscidInstall"`;
			
			print STDERR "$TestComputer: Change permissions on test scripts in /tmp/$WorkingRep/Test/\n";
			`ssh $TestComputer "chmod 755 /tmp/$WorkingRep/Test"`;
	
			# set that it is not tested successfully
			$Tested = 0;
			foreach $TraceFlag ( ('1', '0') )
			{
				$OptionString = "\@OptionsTable = $Options{$TestComputer};";
				eval( $OptionString );

				foreach $Option ( @OptionsTable )
				{
					# &WakeOnLan($TestComputer);
					
					$TestSuite .= "CMT: Test Omiscid with 'debug=$DebugFlag trace=$TraceFlag $Option' flags.\n";
					
					print STDERR "$TestComputer: Try to install Omiscid with 'debug=$DebugFlag trace=$TraceFlag $Option' flags.\n";
					&AddLog( "TRY: $TestComputer: Try to install Omiscid with 'debug=$DebugFlag trace=$TraceFlag $Option' flags." );
					$LastTry = "ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileOmiscid.sh $WorkingRep 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=$InstallOmiscidFolder'\""; 
					&AddLog( "CMD: ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileOmiscid.sh $WorkingRep 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=$InstallOmiscidFolder'\"" ); 
					$Tested = &CheckCommand( "ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileOmiscid.sh $WorkingRep 'debug=$DebugFlag trace=$TraceFlag $Option install prefix=$InstallOmiscidFolder'\"", 'scons: done building targets.' );
					if ( $Tested == 0 )
					{
						&AddLog( "ERR: Could not compile Omiscid on $TestComputer" );
						$TestSuite .= "IFA: Could not compile and install Omiscid.\n";
						next;
					}
					else
					{
						$TestSuite .= "IOK: Compile and install Omiscid.\n";
					}					
					# set that it is not tested successfully
					$Tested = 0;
			
					print STDERR "$TestComputer: Try to compile and run test(s) with 'debug=$DebugFlag trace=$TraceFlag $Option'.\n";
					foreach $test ( keys %TestsList )
					{
						# try to compile Test Examples
						$LastTry = "ssh $TestComputer $Options{$TestComputer} : \"/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test' '$Compiler' '$CompilerOptions' '$InvoqueDebugger'\"";
						$Tested = 0;
						$NbTestsTried = 0;
						$NbTestsOk = 0;
						while( $NbTestsTried < 5 )
						{
							$NbTestsTried++;
							# &WakeOnLan($TestComputer);
							print STDERR "$TestComputer: Test $NbTestsTried.\n";
							&AddLog( "TRY: $TestComputer: Test $NbTestsTried." );
							&AddLog( "CMD: \"ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test TimeoutProg.cpp' '$Compiler' '$CompilerOptions' '$InvoqueDebugger'\"" );
							$res = &CheckCommand( "ssh $TestComputer \"/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test TimeoutProg.cpp' '$Compiler' '$CompilerOptions' '$InvoqueDebugger'\"", 'Test ok\.' );
							if ( $res == 1 )
							{
								# test is ok
								$NbTestsOk++;
								last;
							}
							sleep( 2 );

						}
						if ( $NbTestsTried == $NbTestsOk )
						{
							
							$Tested = 1;
							$TestSuite .= "TOK: Compile $TestsList{$test} and run it successfully ($NbTestsOk/$NbTestsTried)\n     files used '$test'\n";
							&AddLog( "TOK: Compile $TestsList{$test} and run it successfully ($NbTestsOk/$NbTestsTried)\n     files used '$test'" );
						}
					
						if ( $Tested == 0 )
						{
							$TestSuite .= "TFA: Could not compile $TestsList{$test} and run it successfully ($NbTestsOk/$NbTestsTried)\n     files used '$test'\n";
							&AddLog( "TFA: Could not compile $TestsList{$test} and run it successfully ($NbTestsOk/$NbTestsTried)\n     files used '$test'" );
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
					`ssh $TestComputer "rm -rf $InstallOmiscidFolder"`;
				}
			}
		}
		# cleaning up everything
		print STDERR "$TestComputer: Cleanup /tmp/\n";
		`ssh $TestComputer "cd /tmp/;rm -rf $WorkingRep OmiscidInstall $VersionFile"`;
		
		$TestSuite .= "\n";
	}
}
else
{
	$TestSuite .= "No test done.\n";
}

if ( &LogOk() == 0 )
{
	print STDERR "\n\n\t=> Problem when testing $VersionFile.\n";
	exit();
}

&AddLog( "TOK: All tests passed." );

`rm ../$VersionFile`;

print STDERR "\n\n\t=> $VersionFile successfully tested.\n";
`echo $Version > ../LastVersion.info`;
print STDERR "Generate final version of $VersionFile.\n";

# generate report of tests
open( $fdtests, '>TESTS' );
print $fdtests $TestSuite;
close( $fdtests);

# generate date
my ($sec,$min,$hour,$mday,$mon,$year, $wday,$yday,$isdst) = localtime time;
$year += 1900;
$mon += 1;
my $fdchanges;
my $ligne;
my $contenu = '';
	
open( $fd, '<CHANGES.in' ) or die "Could not open 'CHANGES.in'\n";
while( $ligne = <$fd> )
{
	$ligne =~ s/[\r\n\s]+$//g;
	$ligne =~ s/%Version%/$Version/g;
	$ligne =~ s/%Date%/$year-$mon-$mday/g;
	$contenu .= $ligne . '_#_#_#_#_';
}
close( $fd );
	
$contenu =~ s/(_#_#_#_#_)+$//;
$contenu =~ s/(_#_#_#_#_)/\n/g;
	
open( $fd, '>CHANGES' ) or die "Could not write to 'CHANGES'\n";
print $fd $contenu;
print $fd "\n";
close( $fd );

# reset file list
undef(%FilesToAdd);

if ( -e 'Doc' && -d 'Doc' )
{
	print "Remove Doc\n";
	`rm -rf Doc`;
}

if ( $DoDoc == 1 )
{
	print "Generate Doc\n";
	`doxygen`;
}

`perl RecursiveDos2Unix.pl .`;

# generate testing archive
&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Examples/");
if ( $DoDoc == 1 )
{
	&RecurseWork::RecurseWork("Doc/");
}

print STDERR "Remove BipService.cpp & TimeoutProg.cpp from list\n";
undef($FilesToAdd{'Examples/BipService.cpp'});
undef($FilesToAdd{'Examples/TimeoutProg.cpp'});

$command = "zip -9 $VersionFile ";
foreach $file ( @UsualFiles )
{
	$command .= "$WorkingRep/$file ";
}
foreach $file ( keys %FilesToAdd )
{
	if ( $FilesToAdd{$file} == 0 )
	{
		next;
	}
	$command .= "$WorkingRep/$file ";
}

# print $command;

chdir('..');
system( $command );
chdir($WorkingRep);
print STDERR "=> done.\n";
