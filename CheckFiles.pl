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

sub AddEmptyLineAtEnd()
{
	my $FileName = shift @_;
 	my $fd;

 	open( $fd, ">>$FileName" ) or return @FileContent;
	print $fd "\r\n";
	close( $fd );
}

sub LoadFile()
{
	my $FileName = shift @_;
	my $CurrentLine;
 	my @FileContent = ();
 	my $fd;
 		
 	open( $fd, "<$FileName" ) or return @FileContent;
	while( $CurrentLine = <$fd> )
	{
		$CurrentLine =~ s/[\r\n]+$//;
		push @FileContent, $CurrentLine;
	}
	close( $fd );
	
	return @FileContent;
}

sub LogCurrentFileVersion()
{
	my $CurrentLine;
	my $CurrentLineNumber = 0;
 	my $FileToPrintContent = shift @_;
 	
 	foreach $CurrentLine ( @$FileToPrintContent )
 	{
 		$CurrentLineNumber++;
 		print $CurrentLineNumber . ': ' . $CurrentLine . "\n";
 	}
	
	return;
}

sub RemoveConstantStringFromFile()
{
	my $FileIn = shift @_;
	my $CurrentLineNumber;
	
	for ( $CurrentLineNumber = 0; defined @$FileIn[$CurrentLineNumber]; $CurrentLineNumber++ )
	{
		@$FileIn[$CurrentLineNumber] =~ s/\\\"//g;
		while ( @$FileIn[$CurrentLineNumber] =~ s/\"([^\"]+)\"/\"\"/ ) {}
	}
}

sub RemoveCommentsFromFile()
{
	my $FileIn = shift @_;
	my $CurrentLine;
 	my $Mode = 0;  # 0 means normal mode, 1 means multine
 	my $StartOfLine;
 	my $EndOfLine;
 	my $LineProcessed;
 	my $CurrentLineNumber;
	
	for( $CurrentLineNumber = 0; defined @$FileIn[$CurrentLineNumber]; $CurrentLineNumber++ )
	{
		$CurrentLine = @$FileIn[$CurrentLineNumber];
		
		$LineProcessed = 0;
		while( $LineProcessed == 0 )
		{
			if ( $Mode == 0 )
			{
				# "/*" ?
				if ( @$FileIn[$CurrentLineNumber] =~ /^(.*)\/\*(.*)$/ )
				{
					$StartOfLine = $1;
					$EndOfLine = $2;
					
					if ( $StartOfLine =~ /^(.*)\/\// )
					{
						@$FileIn[$CurrentLineNumber] = $1;
						$LineProcessed = 1;
						next;
					}
					
					# die "'$StartOfLine' '$EndOfLine'";
					
					# ok, we have /*, remove it and go on
					$Mode = 1;
					@$FileIn[$CurrentLineNumber] = $StartOfLine;
					next;
				}
				if ( $CurrentLine =~ /^(.*)\/\// )
				{
					@$FileIn[$CurrentLineNumber] = $1;
					$LineProcessed = 1;
					
					# die "'$FileContent'";
					next;
				}
				
				# nothing special on this line
				$LineProcessed = 1;
				next;
			}
			else
			{
				# die "$CurrentLine";
				# we are looking for "*/"
				if (@$FileIn[$CurrentLineNumber] =~ /^(.*)\*\/(.*)$/ )
				{
					@$FileIn[$CurrentLineNumber] = $2;
					$Mode = 0;
					next; # go on on this line
				}
				else
				{
					# no "*/", skip this line
					@$FileIn[$CurrentLineNumber] = '';
					$LineProcessed = 1;
					next;
				}
			}
		}
	}
}

sub CheckEmptyLineAtEnd()
{
 	my $FileName = shift @_;
	my @FileContent;
 	my $CurrentLine;
 	my $PreviousLineWasEndedByReturn;
 	
	$PreviousLineWasEndedByReturn = 0;
	
	@FileContent = &LoadFile( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		if ( $CurrentLine eq '' )
		{
			$PreviousLineWasEndedByReturn = 1; 
		}
		else
		{
			$PreviousLineWasEndedByReturn = 0; 
		}
	}
	
	if ( $PreviousLineWasEndedByReturn == 0 )
	{
		print "$FileName: you should add an empty line at end to be gcc compliant\n";
		&AddEmptyLineAtEnd( $FileName );
	}
}

sub CheckIfDef()
{
 	my $FileName = shift @_;
 	my $ShortFileName = shift @_;
	my @FileContent;
 	my $CurrentLine;
 	my $LineNumber;
 	my $CurrentHeader;
 	my $EndFound;
 	my $ExpectedHeader;
 	my $Folder;
 	
 	my @ExcludedFiles = ( 'System/ConfigSystem.h', 'Com/ConfigCom.h', 'ServiceControl/ConfigServiceControl.h', 'System/TrackingMemoryLeaks.h', 'Examples/Calculator/', 'Json/' );
 	my $CurrentFile;
 	
 	foreach $CurrentFile ( @ExcludedFiles )
 	{
 		# '/' => '\/'
 		$CurrentFile =~ s/\//\\\//g;
	 	if ( $FileName =~ /$CurrentFile/ )
	 	{
	 		return;
	 	}
	}
 	
 	$FileName =~ /([^\/]+)\/[^\/]+$/;
 	$Folder = $1;

	$ExpectedHeader = $ShortFileName;
 	$ExpectedHeader =~ s/([A-Z]+)/_$1/g;
 	$ExpectedHeader =~ s/.h$/_H__/;
 	$ExpectedHeader = '_' . $ExpectedHeader;
 	$ExpectedHeader =~ tr/a-z/A-Z/;
 	
	@FileContent = &LoadFile ( $FileName );
	
	for( $LineNumber = 0; defined $FileContent[$LineNumber]; $LineNumber++ )
	{
		$CurrentLine = $FileContent[$LineNumber];
		
		if ( $CurrentLine =~ /^\#ifndef/ )
		{
			$CurrentLine =~ /^\#ifndef\s+(\w+)/;
			$CurrentHeader = $1;
			if ( !($CurrentHeader eq $ExpectedHeader) )
			{
				print "$FileName: bad prepocessor definition (is '$CurrentHeader' expected '$ExpectedHeader')\n";				
			}
			$Headers{$CurrentHeader}++;
			$LineNumber++;
			$CurrentLine = $FileContent[$LineNumber];
			if ( !($CurrentLine =~ /^\#define\s+$CurrentHeader[\s]*$/) )
			{
				print "$FileName: bad #ifndef/#define sequence ('$CurrentLine')\n";
				&LogCurrentFileVersion( \@FileContent );
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
	for( ; defined $FileContent[$LineNumber]; $LineNumber++ )
	{
		$CurrentLine = $FileContent[$LineNumber];
				
		if ( $CurrentLine =~ /^\#endif\s+\/\/\s+$CurrentHeader[\s]*$/ )
		{
			$EndFound = 1;
			$LineNumber++;
			last;
		}
	}
	if ( $EndFound == 1 )
	{
		for( ; defined $FileContent[$LineNumber]; $LineNumber++ )
		{
			$CurrentLine = $FileContent[$LineNumber];
		
			if ( $CurrentLine eq '' )
			{
			}
			else
			{
				print "$FileName: something found ('$CurrentLine') after '#endif // $CurrentHeader'\n";
			}
		}
	}
	else
	{
		print "$FileName: '#endif // $CurrentHeader' not found\n";
		&LogCurrentFileVersion ( \@FileContent );
		die;
	}
}


sub FirstIncludeofHeaderFileisConfig()
{
 	my $FileName = shift @_;
	my @FileContent;
 	my $CurrentLine;
 	my $FirstIncludedFile;
 	my $Folder;
 
  	my @ExcludedFiles = ( 'System/ConfigSystem.h', 'Com/ConfigCom.h', 'ServiceControl/ConfigServiceControl.h', 'ServiceControl/UserFriendlyAPI.h', 'System/TrackingMemoryLeaks.h' );
 	my $CurrentFile;
 	
 	if ( $FileName =~ /^(Examples|Json)\// )
 	{
 		return;
 	}
 	
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

	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		# $CurrentLine =~ s/[\n]+$//;
		# print "##\n$CurrentLine\n##\n";
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$FirstIncludedFile = $1;
			if ( $FirstIncludedFile =~ /$Folder\/Config${Folder}.h$/ )
			{
				last;
			}
			# print "$CurrentLine : '$FirstIncludedFile'\n";
			print "$FileName: the first include of an header file *must* be '$Folder/Config${Folder}.h'\n";
			return;
		}
		else
		{
			# print "no\n"
		}
	}
}

sub FirstIncludeofSourceFileisItsHeader()
{
 	my $FileName = shift @_;
	my $FileContent;
 	my $CurrentLine;
 	my $FirstIncludedFile;
 	my $ExpectedInclude;
 	my $ExpectedIncludeWithQuotes;
 	my $Rep;
 	my $ShortFileName;
 	
 	if ( $FileName =~ /^([^\/]+)\/([^\/]+)\.cpp$/ )
 	{
 		$Rep = $1;
 		$ShortFileName = $2;
 	
 	 	# print "$FileName\n$Rep\n$ShortFileName\n" ;
 	 	$ExpectedInclude = "$Rep/$ShortFileName.h";
 	 	$ExpectedIncludeWithQuotes = "$ShortFileName.h";
 	 }
 	 else
 	 {
 	 	print STDERR "Skip '$FileName'\n";
 	 	return;
 	 }

	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		# $CurrentLine =~ s/[\n]+$//;
		# print "##\n$CurrentLine\n##\n";
		if ( $CurrentLine =~ /^\#include\s+\"([^\"]+)\"/ )
		{
			$FirstIncludedFile = $1;
			if ( $FirstIncludedFile ne $ExpectedIncludeWithQuotes )
			{
				print "$FileName: the first include *must* be '$ExpectedIncludeWithQuotes' (is '$FirstIncludedFile')\n";
			}
			last;
		}
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$FirstIncludedFile = $1;
			if ( $FirstIncludedFile ne $ExpectedInclude && -e $ExpectedInclude )
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
}

@StdLibs = ( 'stdio.h', 'stdlib.h' );

sub FirstDoesNotIncludeStdLibs()
{
 	my $FileName = shift @_;
	my @FileContent;
 	my $CurrentLine;
 	my $lib;
 	
 	my @ExcludedFiles = ( 'System/ConfigSystem.h', 'System/TrackingMemoryLeaks.h', 'System/TrackingMemoryLeaks.cpp' );
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
 	
	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		# $CurrentLine =~ s/[\n]+$//;
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
}

sub FilesShouldContainOnlyOneClassDeclaration()
{
 	my $FileName = shift @_;
	my $ClassCount = 0;
 	my @FileContent;
 	
 	if ( $FileName =~ /System\/Config.h$/ )
 	{
 		return;
 	}
 	
	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
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
	
	if ( $ClassCount > 1 )
	{
		print "$FileName: check if there are not more than 1 class definition.\n";
	}
}

sub GenerateIncludeGraph()
{
 	my $FileName = shift @_;
  	my $ShortFileName = shift @_;
  	my $CurrentLine;
 	my @FileContent;
	
 	if ( $FileName =~ /System\/Config.h$/ )
 	{
 		return;
 	}
 	
	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		if ( $CurrentLine =~ /^\#include\s+\<([^\>]+)\>/ )
		{
			$IncludeGraphe{$ShortFileName} .= "$1;";
		}
	}
}

sub ConstantStringCount()
{
 	my $FileName = shift @_;
	my @FileContent;
 	my $CurrentLine;
 	my $String;
 	
	$PreviousLineWasEndedByReturn = 0;
	
	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		while ( $CurrentLine =~ /^[^\"]*\"([^\"]*)\"(.+)[\n]*$/ )
		{
			$String = $1;
			$CurrentLine = $2;
			
			$StringsTable{$String}++;
			
			# print "\"$String\"\n";
		}
	}
}

sub CheckExceptionConsistancy()
{
 	my $FileName = shift @_;
   	my $CurrentLine;
  	my $CurrentLineNumber = 0;
  	my @FileContent;
 	
   	my @ExcludedFiles = ( 'System/TrackingMemoryLeaks.h', 'System/TrackingMemoryLeaks.cpp' );
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
	
 	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		$CurrentLineNumber++;
		
		$CurrentLine =~ s/(\/\/.*)$//;
		$CurrentLine =~ s/(\/\*.*)$//;
		
		if ( $CurrentLine =~ /^throw\s+/ || $CurrentLine =~ /\s+throw\s+/ )
		{
			if ( $CurrentLine =~ /throw\s+[\(\w]+Exception\W/ || $CurrentLine =~ /throw\s+e\W/ )
			{
			}
			else
			{
				print "$FileName ($CurrentLineNumber): bad throw patern ?\n";
			}
		}
		
		if ( $CurrentLine =~ /catch\s*\(([^)]+)\)/ )
		{
			$CurrentLine = $1;
			if ( $CurrentLine =~ /Exception\s*/ || $CurrentLine =~ /\.\.\./ )
			{
			}
			else
			{
				print "$FileName ($CurrentLineNumber): bad catch patern ?\n";
			}
			next;
		}
		
	}
}

sub LockAndUnlockAreProtectedUsingSmartLocker()
{
 	my $FileName = shift @_;
 	my $CurrentLine;
  	my $CurrentLineNumber = 0;
 	my @FileContent;
 	  	
  	my @ExcludedFiles = ( 'System/LockManagement.cpp', 'System/LockManagement.h', 'System/TrackingMemoryLeaks.cpp', 'System/SimpleString.cpp' );
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
	
 	@FileContent = &LoadFile ( $FileName );
	foreach $CurrentLine ( @FileContent )
	{
		$CurrentLineNumber++;
		
		# We do not remove comments for this (see SimpleString.cpp SL_ comments)
		# $CurrentLine =~ s/(\/\/.*)$//;
		# $CurrentLine =~ s/(\/\*.*)$//;

		if ( $CurrentLine =~ /(\.|->)(Lock|Unlock|EnterMutex|LeaveMutex)\(\)/ )
		{
			if ( $CurrentLine =~ /SL_/ )
			{
			}
			else
			{
				print "$FileName ($CurrentLineNumber): bad Lock/Unlock or EnterMutex/LeaveMutex Patern ?\n";
			}
		}
	}
}

sub CheckThatAll_new_CallsAreInstrumented()
{
 	my $FileName = shift @_;
 	my @FileContent;
 	my $CurrentLine;
  	my $CurrentLineNumber = 0;
  	my $fd;
  	
  	# load file
  	@FileContent = &LoadFile( $FileName );
  	
  	# Remove comments and Constant Strings
  	&RemoveCommentsFromFile( \@FileContent );
  	
  	# &LogCurrentFileVersion( \ @FileContent );
  	
  	&RemoveConstantStringFromFile( \@FileContent );
  	  	
  	# &LogCurrentFileVersion( \ @FileContent );
  	
	foreach $CurrentLine ( @FileContent )
	{
		$CurrentLineNumber++;
		
		if ( $CurrentLine =~ /new\s+/ )
		{
			if ( $CurrentLine =~ /new\s+OMISCID_TLM/ )
			{
			}
			else
			{
				print "$FileName ($CurrentLineNumber:'$CurrentLine'): bad new Patern ?\n";
				# print $FileContent;
			}
		}
	}
}

# 
sub ConvertFiles()
{
 	my $FileName = shift @_;

	# `dos2unix $FileName`;
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
		
		# Chack that all new are instrumented
		# print "$CompleteFileName (CheckThatAll_new_CallsAreInstrumented)\n";
		&CheckThatAll_new_CallsAreInstrumented($CompleteFileName);
		
		# Check if we have an empty line at end for gcc
		# print "$CompleteFileName (CheckEmptyLineAtEnd)\n";
		&CheckEmptyLineAtEnd($CompleteFileName);
		
		# &ConstantStringCount($CompleteFileName);
		
		# Check for unattended include (done in Config.h)
		&FirstDoesNotIncludeStdLibs($CompleteFileName);
		
		# check thow consistancy
		&CheckExceptionConsistancy($CompleteFileName);
		
		# check Lock/Unlock paradigms
		&LockAndUnlockAreProtectedUsingSmartLocker($CompleteFileName);
		
		# Convert files to Windows CR/LF mode in order to prevent
		# problems using VC2003 and VC2005
		&ConvertFiles($CompleteFileName);
	}
	else
	{
		# print "Do not process $CompleteFileName\n";
	}
}

&RecurseWork::RecurseWork( 'System', 0 );
&RecurseWork::RecurseWork( 'Com', 0 );
&RecurseWork::RecurseWork( 'ServiceControl', 0 );
&RecurseWork::RecurseWork( 'Examples', 0 );
&RecurseWork::RecurseWork( 'Messaging', 0 );
&RecurseWork::RecurseWork( 'Json', 0 );

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
