package RecurseWork;
our @EXPORT = ('RecurseWork', 'RecurseWork_DEBUG');

# Call RecurseWork, it will parse all folder and SubFoler and call
# a WorkOnFile for each file in all rep and subrep...
# Here is a proto for WorkOnFile
# 
# sub WorkOnFile()
# {
# 	my $FileName = shift @_;
# 	my $UserData = shift @_;
# 
# 	work to be done on file
# 
# }
# Optionnal function
# sub WorkOnDirectory()
# {
# 	my $DirName = shift @_;
# 	my $UserData = shift @_;
# 
# 	work to be done on file
# 
# }

# sub EnterDirectory()
# {
# 	my $DirName = shift @_;
# 	my $UserData = shift @_;
# 
# 	return 1;
# }

# Be sur not to use the 'RecurseWork_DIR' file handle because it is used by RecurseWork

my $RecurseWork_DEBUG = 0;

sub RecurseWork()
{
	my $CurrentFolder = shift @_;
 	my $UserData 	  = shift @_;
	my $CurrentEntry;
	my @ListeEntries;
	
	$CurrentFolder =~ s/\/+$//;
	
	print STDERR "$CurrentFolder\n" if $RecurseWork::DEBUG;
	
	if ( defined &main::EnterDirectory )
	{
		# We can choose if we recurse into directory
		if ( &main::EnterDirectory( $CurrentFolder . '/', $UserData ) == 0 )
		{
			return;
		}
	}
	if ( defined &main::WorkOnDirectory )
	{
		# print STDERR "Work on '$CurrentEntry'\n" if $RecurseWork::DEBUG;
		&main::WorkOnDirectory( $CurrentFolder, $UserData );
	}
 			
	opendir( RecurseWork_DIR, $CurrentFolder ) or return;
	while( $CurrentEntry = readdir(RecurseWork_DIR) )
	{
		# do not reprocess this folder and parent one...
		next if ( $CurrentEntry =~ /^\.\.?$/ );
		
		if ( $CurrentFolder =~ /\/$/ )
		{
			push @ListeEntries, "$CurrentFolder$CurrentEntry";
		}
		else
		{
			push @ListeEntries, "$CurrentFolder/$CurrentEntry";
		}			
	}
	closedir( RecurseWork_DIR );
	
	foreach $CurrentEntry ( @ListeEntries )
	{
		if ( -d $CurrentEntry )
		{
			# Always recurse... check is done at the begining
			&RecurseWork( $CurrentEntry . '/', $UserData );
		}
		else
		{
			# it is a file, call WorkOnFile
			# print STDERR "Work on '$CurrentEntry'\n" if $RecurseWork::DEBUG;
			&main::WorkOnFile( $CurrentEntry, $UserData );
		}
	}
}

1;