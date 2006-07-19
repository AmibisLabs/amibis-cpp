require RecurseWork;

sub EnterDirectory()
{
 	my $DirName = shift @_;
 	my $UserData = shift @_;
 
 	if ( $DirName =~ /\.svn\/?$/ || $DirName =~ /^ServiceControl\/test/ )
 	{
 		return 0;
 	}
 	# print "$DirName ok\n";
 	return 1;
}

sub CheckEmptyLineAtEnd()
{
 	my $FileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $PreviousLineWasEndedByReturn;
 	
	$PreviousLineWasEndedByReturn = 0;
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		if ( $CurrentLine =~ /[\r\n]+$/ )
		{
			$PreviousLineWasEndedByReturn = 1; 
		}
		else
		{
			$PreviousLineWasEndedByReturn = 0; 
		}
	}
	close( $fd );
	
	if ( $PreviousLineWasEndedByReturn == 0 )
	{
		print "$FileName: you should add an empty line at end to be gcc compliant\n";
	}
}

sub CheckIfDef()
{
 	my $FileName = shift @_;
 	my $ShortFileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $CurrentHeader;
 	my $EndFound;
 	my $ExpectedHeader;
 	my $Folder;
 	
 	$FileName =~ /([^\/]+)\/[^\/]+$/;
 	$Folder = $1;

	if ( $FileName =~ /\/Config.h$/ )
	{
 		$ExpectedHeader = "${Folder}$ShortFileName";
 	}
 	else
 	{
 		$ExpectedHeader = $ShortFileName;
 	}
 	
 	$ExpectedHeader =~ s/([A-Z]+)/_$1/g;
 	$ExpectedHeader =~ s/.h$/_H__/;
 	$ExpectedHeader = '_' . $ExpectedHeader;
 	$ExpectedHeader =~ tr/a-z/A-Z/;
 	
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		if ( $CurrentLine =~ /^\#ifndef/ )
		{
			$CurrentLine =~ /^\#ifndef\s+(\w+)/;
			$CurrentHeader = $1;
			if ( !($CurrentHeader eq $ExpectedHeader) )
			{
				print "$FileName: bad prepocessor definition (is '$CurrentHeader' expected '$ExpectedHeader')\n";				
			}
			$Headers{$CurrentHeader}++;
			$CurrentLine = <$fd>;
			if ( !($CurrentLine =~ /^\#define\s+$CurrentHeader[\s\r\n]+$/) )
			{
				print "$FileName: bad #ifndef/#define sequence\n";
			}
			# Get out the loop
			last;
		}
		if ( $CurrentLine =~ /^\#/ )
		{
			print "$FileName: preprocessor commande before #ifndef/#define sequence\n";
		}
	}
	
	# look fot the end of include prevention
	$EndFound = 0;
	while( $CurrentLine = <$fd> )
	{
		if ( $CurrentLine =~ /^\#endif\s+\/\/\s+$CurrentHeader[\s\r\n]+$/ )
		{
			$EndFound = 1;
			last;
		}
	}
	if ( $EndFound == 1 )
	{
		while( $CurrentLine = <$fd> )
		{
			if ( !($CurrentLine =~ /^[\s\r\n]+$/) )
			{
				print "$FileName: something found after '#endif // $CurrentHeader'\n";
			}
		}
	}
	else
	{
		print "$FileName: '#endif // $CurrentHeader' not found\n";
	}
	close( $fd );
}


sub FirstIncludeofHeaderFileisConfig()
{
 	my $FileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $FirstIncludedFile;
 	my $Folder;
 
  	my @ExcludedFiles = ( 'System/Config.h', 'Com/Config.h', 'ServiceControl/Config.h', 'System/TrackingMemoryLeaks.h' );
 	my $CurrentFile;
 	
 	foreach $CurrentFile ( @ExcludedFiles )
 	{
 		# '/' => '\/'
 		$CurrentFile =~ s/\//\\\//g;
	 	if ( $FileName =~ /$CurrentFile$/ )
	 	{
	 		return;
	 	}
	}
 	
 	$FileName =~ /([^\/]+)\/[^\/]+$/;
 	$Folder = $1;

	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		# $CurrentLine =~ s/[\r\n]+$//;
		# print "##\n$CurrentLine\n##\n";
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$FirstIncludedFile = $1;
			if ( $FirstIncludedFile =~ /$Folder\/Config.h$/ )
			{
				last;
			}
			# print "$CurrentLine : '$FirstIncludedFile'\n";
			print "$FileName: the first include of an header file *must* be '$Folder/Config.h'\n";
		}
		else
		{
			# print "no\n"
		}
	}
	
	close( $fd );
}

sub FirstIncludeofSourceFileisItsHeader()
{
 	my $FileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $FirstIncludedFile;
 	my $ExpectedInclude;
 	my $Rep;
 	my $ShortFileName;
 	
 	if ( $FileName =~ /^([^\/]+)\/([^\/]+)\.cpp$/ )
 	{
 		$Rep = $1;
 		$ShortFileName = $2;
 	
 	 	# print "$FileName\n$Rep\n$ShortFileName\n" ;
 	 	$ExpectedInclude = "$Rep/$ShortFileName.h";
 	 }
 	 else
 	 {
 	 	print STDERR "Skip '$FileName'\n";
 	 	return;
 	 }

	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		# $CurrentLine =~ s/[\r\n]+$//;
		# print "##\n$CurrentLine\n##\n";
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$FirstIncludedFile = $1;
			if ( $FirstIncludedFile ne $ExpectedInclude )
			{
				# print "$CurrentLine : '$FirstIncludedFile'\n";
				print "$FileName: the first include *must* be '$ExpectedInclude' (is '$FirstIncludedFile')\n";
			}
			last;
		}
		else
		{
			# print "no\n"
		}
	}
	
	close( $fd );
}

@StdLibs = ( 'stdio.h', 'stdlib.h' );

sub FirstDoesNotIncludeStdLibs()
{
 	my $FileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $lib;
 	
 	my @ExcludedFiles = ( 'System/Config.h', 'System/TrackingMemoryLeaks.h', 'System/TrackingMemoryLeaks.cpp' );
 	my $CurrentFile;
 	
 	foreach $CurrentFile ( @ExcludedFiles )
 	{
 		# '/' => '\/'
 		$CurrentFile =~ s/\//\\\//g;
	 	if ( $FileName =~ /$CurrentFile$/ )
	 	{
	 		return;
	 	}
	}
 	
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		# $CurrentLine =~ s/[\r\n]+$//;
		# print "##\n$CurrentLine\n##\n";
		foreach $lib ( @StdLibs )
		{
			if ( $CurrentLine =~ /^\#include\s+\<$lib\>/i )
			{
				print "$FileName: *must* not include '$lib'\n";
			}
			else
			{
				# print "no\n"
			}
		}
	}
	
	close( $fd );
}

sub FilesShouldContainOnlyOneClassDeclaration()
{
 	my $FileName = shift @_;
	my $ClassCount = 0;
 	
 	if ( $FileName =~ /System\/Config.h$/ )
 	{
 		return;
 	}
 	
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		if ( $CurrentLine =~ /^class\s+/ || $CurrentLine =~ /\s+class\s+/ )
		{
			if ( $CurrentLine =~ /class\s+\w+\s*;/ )
			{
				next;
			}
			
			$ClassCount++;
		}
	}
	
	close( $fd );
	
	if ( $ClassCount > 1 )
	{
		print "$FileName: check if there are not more than 1 class definition.\n";
	}
}

sub GenerateIncludeGraph()
{
 	my $FileName = shift @_;
  	my $ShortFileName = shift @_;
	
 	if ( $FileName =~ /System\/Config.h$/ )
 	{
 		return;
 	}
 	
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$IncludeGraphe{$ShortFileName} .= "$1;";
		}
	}
	
	close( $fd );
}

sub ConstantStringCount()
{
 	my $FileName = shift @_;
	my $fd;
 	my $CurrentLine;
 	my $String;
 	
	$PreviousLineWasEndedByReturn = 0;
	open( $fd, "<$FileName" ) or return;
	while( $CurrentLine = <$fd> )
	{
		while ( $CurrentLine =~ /^[^\"]*\"([^\"]*)\"(.+)[\r\n]*$/ )
		{
			$String = $1;
			$CurrentLine = $2;
			
			$StringsTable{$String}++;
			
			# print "\"$String\"\n";
		}
	}
	close( $fd );
}


sub WorkOnFile()
{
 	my $CompleteFileName = shift @_;
 	my $UserData = shift @_;
 	
	my $Layer;
	my $FilePrefix;
	my $FileExt;
	my $FileName;

	if ( $CompleteFileName =~ /([^\/]+)\/([^\/]+)\.(h|cpp)$/ )
	{
		$Layer 	    = $1;
		$FilePrefix = $2;
		$FileExt    = $3;
		
		$FileName   = $FilePrefix . '.' . $FileExt;
		
		# print "'$Layer'\t'$FilePrefix'\t'$FileExt'\n";
		
		if ( $FileExt eq 'h' )
		{
			# print "$CompleteFileName (CheckIfDef)\n";
			&CheckIfDef($CompleteFileName, $FileName);
			
			# print "$CompleteFileName (FirstIncludeofHeaderFileisConfig)\n";
			&FirstIncludeofHeaderFileisConfig($CompleteFileName);
			
			# &GenerateIncludeGraph($CompleteFileName, $FileName);
		}
		else
		{
			# .cpp file
			# print "$CompleteFileName (FirstIncludeofSourceFileisItsHeader)\n";
			&FirstIncludeofSourceFileisItsHeader($CompleteFileName);
		}
		
		# Check if we have an empty line at end for gcc
		# print "$CompleteFileName (CheckEmptyLineAtEnd)\n";
		&CheckEmptyLineAtEnd($CompleteFileName);
		
		&ConstantStringCount($CompleteFileName);
		
		# Check for unattended include (done in Config.h)
		&FirstDoesNotIncludeStdLibs($CompleteFileName);
	}
	else
	{
		# print "Do not process $CompleteFileName\n";
	}
}


&RecurseWork::RecurseWork( 'System', 0 );
&RecurseWork::RecurseWork( 'Com', 0 );
&RecurseWork::RecurseWork( 'ServiceControl', 0 );

foreach $prep ( keys %Headers )
{
	if ( $Headers{$prep} > 1 )
	{
		die;
	}
}

foreach $String ( keys %StringsTable )
{
	if ( $StringsTable{$String} > 1 && $String ne '' )
	{
		print "WARNING constant string \"$String\" defined $StringsTable{$String} times. Consider using variable.\n";
	}
}

# open( $IG, '>IncludeGraphe.dot' ) or exit;
# print $IG "digraph SmartRoom {\n";
# foreach $fic ( keys %IncludeGraphe )
# {
# 	@value = split /\;/, $IncludeGraphe{$fic};
# 	foreach $val ( @value )
# 	{
# 		print $IG "\"$fic\" -> \"$val\";\n";
# 	}
# }
# print $IG "}\n";
# close( $IG );
# 
