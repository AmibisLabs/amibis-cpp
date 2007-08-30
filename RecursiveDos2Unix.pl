require RecurseWork;

sub Reformat()
{
	my $FileName = shift @_;
	my $fd;
	my $ligne;
	my $contenu = '';
	
	print STDERR "$FileName\n";
	
	open( $fd, "<$FileName" ) or die "Could not open '$FileName'\n";
	while( $ligne = <$fd> )
	{
		$ligne =~ s/[\r\n\s]+$//g;
		while( $ligne =~ /^(($ReplaceBy)*)($ToReplace)(.*)$/ )
		{
			$ligne = $1 . $ReplaceBy . $4;
		}
		$contenu .= $ligne . '_#_#_#_#_';
	}
	close( $fd );
	
	$contenu =~ s/(_#_#_#_#_)+$//;
	$contenu =~ s/(_#_#_#_#_)/\n/g;
	
	open( $fd, ">$FileName" ) or die "Could not write to '$FileName'\n";
	print $fd $contenu;
	print $fd "\n";
	close( $fd );
}

sub WorkOnFile()
{
 	my $FileName = shift @_;
 	my $UserData = shift @_;

	if ( $FileName =~ /\.(cpp|h|txt|xml|xsd)/ )
	{
		&Reformat($FileName);
	}

	if ( $FileName =~ /^SConstruct|LICENCE|README|CHANGES|Doxyfile$/ )
	{
		&Reformat($FileName);
	}
}

sub EnterDirectory()
{
 	my $DirName = shift @_;
	my $UserData = shift @_;
	
	if ( $DirName =~ /\.svn/ || $DirName =~ /VisualStudio.in|WorkingForOMiSCID|Spec|Xsd/ )
	{
		return 0;
	}
		
	# print "$DirName\n";
 
 	return 1;
}

$ToReplace = "\t";
$ReplaceBy = '    ';

$FolderToWorkOn = '';

$ParamPos = 0;
while ( defined $ARGV[$ParamPos] )
{
	if ( $ARGV[$ParamPos] eq '-reverse' )
	{
		$ToReplace = '    ';
		$ReplaceBy = "\t";
		$ParamPos++;
		next;
	}
	if ( $FolderToWorkOn ne '' )
	{
		die "Bad parameter : folder already defined.\n";
	}
	$FolderToWorkOn = $ARGV[$ParamPos];
	$ParamPos++;
}

if ( $FolderToWorkOn eq '' )
{
	die "Missing parameter : working folder not defined.\n";
}

&RecurseWork::RecurseWork($FolderToWorkOn,'');