require RecurseWork;

sub EnterDirectory()
{
 	my $DirName = shift @_;
 	my $UserData = shift @_;
 
 	if ( $DirName =~ /\.svn\/?$/ || $DirName =~ /^\.\/ServiceControl\/test/ || $DirName =~ /^\.\/Doc\// || $DirName =~ /^\.\/WorkingForOMiSCID\// )
 	{
 		return 0;
 	}
 	# print "$DirName ok\n";
 	return 1;
}

sub CreateFoldersForFiles()
{
	my $CompleteFileName = shift @_;
	my $folder = '';
	my $tmpFolder = '';
	
	while( $CompleteFileName =~ /^([^\/]+)\/(.+)$/ )
	{
		$tmpFolder = $1;
		$CompleteFileName = $2;
		
		# print "$CompleteFileName\n";
		
		if ( $tmpFolder eq '.' )
		{
			next;
		}
		
		if ( $folder eq '' )
		{
			$folder = '../StableVersions/OMiSCID/' . $tmpFolder . '/';
		}
		else
		{
			$folder .= '/' . $tmpFolder;
		}
		
		$folder =~ s/\/\//\//g;
		
		if ( ! defined $CreatedFolders{$folder} || ! -e $folder )
		{
			$CreatedFolders{$folder} = 1;
			print "Create $folder\n";
			mkdir $folder, 0755;
		}
	}
}

sub WorkOnFile()
{
 	my $CompleteFileName = shift @_;
 	my $UserData = shift @_;
 	my $FileOut;
 	
 	print "$CompleteFileName\n";
 	&CreateFoldersForFiles($CompleteFileName);
 	
 	$FileOut = $CompleteFileName;
 	$FileOut =~ s/^\.\//\.\.\/StableVersions\/OMiSCID\//;
 	
 	# print "cp $CompleteFileName $FileOut\n";
 	`cp $CompleteFileName $FileOut`;
}

# Create if not exists ../StableVersions/OMiSCID/
if ( !-e '../StableVersions/OMiSCID/' )
{
	`mkdir ../StableVersions/OMiSCID/`;
}

# copy back files to save from StableVersion
`cp -f ../StableVersions/OMiSCID/Problems.txt ../StableVersions/OMiSCID/Doxygen.log .`;

# remove Old StableVersion
`rm -rf ../StableVersions/OMiSCID/*`;

&RecurseWork::RecurseWork('.','');