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
		while( $ligne =~ /^(($ReplaceBy)*)($ToReplace)(.*)$/ )
		{
			$ligne = $1 . $ReplaceBy . $4;
		}
		$ligne =~ s/[\r\n\s]+$//g;
		$contenu .= $ligne . '_#_#_#_#_';
	}
	close( $fd );
	
	$contenu =~ s/(_#_#_#_#_)+$//;
	if ( $ReverseMode == 0 )
	{
		$contenu =~ s/(_#_#_#_#_)/\n/g;
	}
	else
	{
		$contenu =~ s/(_#_#_#_#_)/\r\n/g;
	}
	
	open( $fd, ">$FileName" ) or die "Could not write to '$FileName'\n";
	print $fd $contenu;
	if ( $ReverseMode == 0 )
	{
		print $fd "\n";
	}
	else
	{
		print $fd "\r\n";
	}
	close( $fd );
}

sub WorkOnFile()
{
 	my $FileName = shift @_;
 	my $UserData = shift @_;

	if ( $FileName =~ /\.(cpp|h|txt|xml|xsd|in\.scons)$/ )
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
 	my $TmpDirName;
	
	if ( $DirName =~ /\.svn/ || $DirName =~ /VisualStudio.in|WorkingForOMiSCID|Spec|Xsd/ )
	{
		return 0;
	}
	
	foreach $TmpDirName ( keys %ToExclude )
	{
		if ( $DirName =~ /^$TmpDirName/ || $DirName =~ /^\.\/$TmpDirName/ )
		{
			return 0;
		}
	}
	
	# print STDERR "$DirName is ok\n";
 
 	return 1;
}

$ToReplace = "\t";
$ReplaceBy = '        ';
$ReverseMode = 0;

@FolderToWorkOn = ();

$ParamPos = 0;
while ( defined $ARGV[$ParamPos] )
{
	if ( $ARGV[$ParamPos] eq '-reverse' )
	{
		$ToReplace = '        ';
		$ReplaceBy = "\t";
		$ReverseMode = 1;
		$ParamPos++;
		next;
	}
	if ( $ARGV[$ParamPos] =~ /^-exclude\=(.+)$/ )
	{
		# print STDERR "$1 is excluded\n";
		$ToExclude{$1} = 1;
		$ParamPos++;
		next;
	}	
	push @FoldersToWorkOn, $ARGV[$ParamPos];
	$ParamPos++;
}

foreach $item ( @FoldersToWorkOn )
{
	
	if ( -d $item )
	{
		&RecurseWork::RecurseWork($item,'');
	}
	else
	{
		&WorkOnFile( $item );
	}
}