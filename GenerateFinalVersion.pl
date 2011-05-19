use IPC::Open3;
use Cwd;

require RecurseWork;
require ComputerManagement;

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
	my $IsOk = 1;
	
	# Empty problem file.
	open( $fdpb, "<$NameOfLogFile" );
	while( $line = <$fdpb> )
	{
		if ( $line =~ /^ERR:/ )
		{
			$IsOk = 0;
			next;
		}
		if ( $line =~ /^  Start test phase on/ )
		{
			# New start, remove previous error (previous try)
			$IsOk = 1;
			next;
		}
	}
	close( $fdpb );	
	return $IsOk;
}

# default values for parameters
$IncrVersionType = 0;	# last
$DoTest = 1;
$DoDoc = 1;
$StopAfterZip = 0;
$DoPackage = 1;

$PosArgv = 0;
while( defined $ARGV[$PosArgv] )
{
	if ( $ARGV[$PosArgv] =~ /^-middle$/i )
	{
		$IncrVersionType = 1; # middle number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-major$/i )
	{
		$IncrVersionType = 2; # major number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-minor$/i )
	{
		$IncrVersionType = 0; # minor number
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-last$/i )
	{
		$IncrVersionType = -1; # regenerate the same version
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-notest$/i )
	{
		$DoTest = 0; # major number
		print "do not test\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-stopafterzip$/i )
	{
		$StopAfterZip = 1; # major number
		print "just generate zip\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-nodoc$/i )
	{
		$DoDoc = 0; # major number
		print "do not generate documentation\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-emptylog$/i )
	{
		&EmptyLog();
		print "log is empty\n";
		$PosArgv++;
		next;
	}
	if ( $ARGV[$PosArgv] =~ /^-nopackage$/i )
	{
		$DoPackage = 0;
		$PosArgv++;
		next;
	}
	die "Wrong parameter\n";
}

@UsualFiles = ( 'SConstruct', 'OmiscidScons.py', 'OmiscidInit.py', 'LICENCE', 'README', 'CHANGES', 'Doxyfile', 'TESTS', 'Omiscid-config.scons.in', 'Omiscid.h' );
$Version = "1.0.0";

$CurrentPath = cwd;
$WorkingRep = $CurrentPath;

$WorkingRep =~ /\/([^\/]+)\/?$/;
$WorkingRep = $1;

if ( $WorkingRep eq '' )
{
	die "Unable to find working rep\n";
}

# $WorkingRep = 'OMiSCID-Dev'; # 'OMiSCID';

# Change current Working directory to a subdirectory
if ( !-e './CreateFinalVersion' )
{
	mkdir './CreateFinalVersion', 0755;
}
chdir('./CreateFinalVersion');

if ( -e 'LastVersion.info' )
{
	open( $fd, '<LastVersion.info' ) or die "Unable to open 'LastVersion.info'\n";
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
if ( -e $VersionFile )
{
	print "Remove $VersionFile\n";
	unlink $VersionFile;
}

# generate date
my ($sec,$min,$hour,$mday,$mon,$year, $wday,$yday,$isdst) = localtime time;
$year += 1900;
$mon += 1;
my $fdchanges;
my $ligne;
my $contenu;
my $FileIn, $FileWithoutIn;

mkdir 'OMiSCID', 0755;

while( $FileIn = <../*.in> )
{
	$FileIn =~ /^\.\.\/(.+)\.in$/;
	$FileWithoutIn = "OMiSCID/$1";
	
	$contenu = '';
	
	print STDERR "Process $FileIn\n";
	
	open( $fd, "<$FileIn" ) or die "Could not open '$FileIn'\n";
	
	while( $ligne = <$fd> )
	{
		if ( $ligne =~ /^\d+\.\d+\.\d+/ )
		{
			$ligne = '_#_#_O_#_#_' . $ligne;
		}
		$ligne =~ s/^(\s+)\[/_#_#_O_#_#_$1\[/;
		$ligne =~ s/[\s\r\n]+$//;
		$ligne =~ s/%OMISCID_VERSION%/$Version/g;
		$ligne =~ s/%OMISCID_DAY%/$year-$mon-$mday/g;
		$ligne =~ s/%OMISCID_YEAR%/$year/g;
		$ligne =~ s/%OMISCID_SONAME_SUFFIX%/$majorn\.$middlen/g;
		$contenu .= $ligne . '_#_#_#_#_';
	}
	close( $fd );
	
	# print STDERR "2\n";
	
	$contenu =~ s/(_#_#_#_#_)+/_#_#_#_#_/g;
	$contenu =~ s/(_#_#_#_#_|_#_#_O_#_#_)/\n/g;
	
	# print STDERR "3\n";

	open( $fd, ">$FileWithoutIn" ) or die "Could not write to '$FileWithoutIn'\n";
	print $fd $contenu;
	print $fd "\n";
	close( $fd );
}

print STDERR "Copying files\n";
$command = "rsync -avzu --exclude=.svn --delete ../System ../Com ../ServiceControl ../Messaging ../Json ../Examples ../Test ";
foreach $file ( @UsualFiles )
{
	$command .= "../$file ";
}
$command .= " OMiSCID";
system( $command );

chdir( $CurrentPath );
`perl RecursiveDos2Unix.pl ./CreateFinalVersion/OMiSCID -exclude=Doc`;

chdir( './CreateFinalVersion/OMiSCID' );

# generate testing archive
&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Messaging/");
&RecurseWork::RecurseWork("Json/");
&RecurseWork::RecurseWork("Examples/");
&RecurseWork::RecurseWork("Test/");

$command = "zip -9 ../$VersionFile ";
foreach $file ( @UsualFiles )
{
	$command .= "$file ";
}
foreach $file ( keys %FilesToAdd )
{
	$command .= "$file ";
}

# print $command

system( $command );

if ( $StopAfterZip == 1 )
{
	exit();
}

chdir( '..' );

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

if ( $DoTest == 1 )
{	
	$TestSuite .= "##################\n";
	$TestSuite .= "# Computer 1\n";
	$TestSuite .= "##################\n";
	$TestSuite .= "CMT: Compilation environment\n";
	$TestSuite .= "C++: Microsoft Visual Studio 2005\n";
	$TestSuite .= "     Version 8.0.50727.867  (vsvista.050727-8600)\n";
	$TestSuite .= "     Microsoft .NET Framework\n";
	$TestSuite .= "     Version 2.0.50727 SP2\n";
	$TestSuite .= "     Édition installée : Professional\n";
	$TestSuite .= "     Microsoft Visual C# 2005\n";
	$TestSuite .= "     Microsoft Visual C++ 2005\n";
	$TestSuite .= "     Microsoft Visual J# 2005\n";
	$TestSuite .= "     Microsoft Web Application Projects 2005\n";
	$TestSuite .= "     Crystal Reports\n";
	$TestSuite .= "     Microsoft Visual Studio 2005 Professional - Français Service Pack 1 (KB926607)\n";
	$TestSuite .= "     Security Update pour Microsoft Visual Studio 2005 Professional - Français (KB937061)\n";
	$TestSuite .= "     Security Update pour Microsoft Visual Studio 2005 Professional - Français (KB947738)\n";
	$TestSuite .= "     Update pour Microsoft Visual Studio 2005 Professional - Français (KB932233)\n";
	$TestSuite .= "     Insure++ Version 7.1.2 (build 2007-09-10)\n";
	$TestSuite .= "XML: libxml2 version libxml2-2.7.3 with iconv-1.9.2\n\n";
	
	
	$TestsList{'RegisterSearchTest.cpp RegisterThread.cpp'} = 'RegisterTest';
	$TestsList{'BrowsingTest.cpp RegisterThread.cpp'} = 'BrowsingTest';
	$TestsList{'SendHugeData.cpp'} = 'SendHugeData';
	
	$InstallOmiscidFolder = '/tmp/OmiscidInstall';
	# $InstallOmiscidFolder = '/usr/local/';
	
	# $PutInEnv = "setenv LD_LIBRARY_PATH $InstallOmiscidFolder/lib/:\$LD_LIBRARY_PATH;setenv DYLD_LIBRARY_PATH $InstallOmiscidFolder/lib/:\$LD_LIBRARY_PATH;setenv PATH $InstallOmiscidFolder/bin:\${PATH}:$InstallOmiscidFolder/bin/";
	
	$NumComputer = 0;
	
	local (*READ, *WRITE, *ERROR);
	
	$NumComputer = 1;
	foreach $TestConfig ( keys %Configs )
	{	
		# Check if computer if available
		if ( &StartComputer($TestConfig) == 0 )
		{
			$Tested = 0;
			&AddLog( "WRN: Could not start and connect to $TestComputer" );
			next;
		}
		
		$TestComputer = $Computer{$TestConfig};
		
		&AddLog( "TOK: Connected to $TestComputer." );
		
		# incr num of computer 1
		$NumComputer++;
		
		$TestSuite .= "\n\n##################\n# Computer $NumComputer ($TestConfig) \n##################\n\n";
		$TestSuite .= "\n\n##################\n# rhea is the network name of all virtual OSes \n##################\n\n";
		
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
			
			if ( $Options{$TestConfig} =~ /debugthread\=(1|yes|true)/ )
			{
				$CompilerOptions .= ' -DDEBUG_THREAD ';
			}
			
			if ( $Options{$TestConfig} =~ /chmem\=(1|yes|true)/ )
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
			system("ssh $TestComputer \"cd /tmp/;rm -rf $WorkingRep OmiscidInstall $VersionFile\"");	
		        
			# copy Archive to the test computer
			print STDERR "$TestComputer: Copy and unzip Omiscid in /tmp/\n";
			`scp ../$VersionFile $TestComputer:/tmp/;ssh $TestComputer "cd /tmp;rm -rf $WorkingRep OmiscidInstall;unzip $VersionFile;mkdir OmiscidInstall"`;
			
			print STDERR "$TestComputer: Change permissions on test scripts in /tmp/$WorkingRep/Test/\n";
			`ssh $TestComputer "chmod 755 /tmp/$WorkingRep/Test"`;
			
			# set that it is not tested successfully
			$Tested = 0;
			foreach $TraceFlag ( ('1', '0') )
			{
				$OptionString = "\@OptionsTable = $Options{$TestConfig};";
				eval( $OptionString );

				foreach $Option ( @OptionsTable )
				{
					# &WakeOnLan($TestComputer);
					
					# die $Option;
					
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
						$LastTry = "ssh $TestComputer $Options{$TestConfig} : \"/tmp/$WorkingRep/Test/CompileAndRunTest.sh $WorkingRep '$test' '$Compiler' '$CompilerOptions' '$InvoqueDebugger'\"";
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
		
		&StopComputer( $TestConfig );
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

`rm $VersionFile`;

print STDERR "\n\n\t=> $VersionFile successfully tested.\n";
`echo $Version > LastVersion.info`;
print STDERR "Generate final version of $VersionFile.\n";

chdir( 'OMiSCID' );
# generate report of tests
open( $fdtests, '>TESTS' );
print $fdtests $TestSuite;
close( $fdtests);

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

# generate testing archive
&RecurseWork::RecurseWork("System/");
&RecurseWork::RecurseWork("Com/");
&RecurseWork::RecurseWork("ServiceControl/");
&RecurseWork::RecurseWork("Messaging/");
&RecurseWork::RecurseWork("Json/");
&RecurseWork::RecurseWork("Examples/");
if ( $DoDoc == 1 )
{
	&RecurseWork::RecurseWork("Doc/");
}

print STDERR "Remove BipService.cpp & TimeoutProg.cpp from list\n";
undef($FilesToAdd{'Examples/BipService.cpp'});
undef($FilesToAdd{'Examples/TimeoutProg.cpp'});

$command = "zip -9 ../$VersionFile ";
foreach $file ( @UsualFiles )
{
	$command .= "$file ";
}
foreach $file ( keys %FilesToAdd )
{
	if ( $FilesToAdd{$file} == 0 )
	{
		next;
	}
	$command .= "$file ";
}

# print $command;
system( $command );

chdir( '..' );

$VersionFile =~ s/\.zip/\.tgz/;
`rm -rf $VersionFile`;
`tar cvfz $VersionFile OMiSCID`;

# Generate package
if ( $DoPackage == 1 )
{
	`perl MakeAllPackages.pl`;
}

print STDERR "=> done.\n";
