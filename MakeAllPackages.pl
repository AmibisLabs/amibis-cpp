# search for current version number in changelog
$CurrentVersion = '';

open( $fd, '<CHANGES' ) or die "unable to open 'CHANGES' file\n";
# seek first line with a sequence number
while( $line = <$fd> )
{
	if ( $line =~ /^(\d+\.\d+)\.(\d+)\s+/ )
	{
		$OMISCID_PACKAGEVERSION = "$1.$2";
		$CurrentVersion = "$1.$2";
		last;
	}
}

close( $fd );


# Generate all packages name

@DaemonNames = ( 'avahi', 'mdns' );
@DebugModes = ( '', '-debug' );
@Archies = ( '-arch=32bits', '-arch=64bits' );

$OriginalReplacesString = '';
for $DaemonName ( @DaemonNames )
{
	for $DebugMode ( @DebugModes )
	{
		if ( $OriginalReplacesString eq '' )
		{
			$OriginalReplacesString = "omiscid-$DaemonName$DebugMode(= $CurrentVersion)";
		}
		else
		{
			$OriginalReplacesString .= ", omiscid-$DaemonName$DebugMode(= $CurrentVersion)";
		}
	}
}

foreach $DaemonName ( @DaemonNames )
{
	foreach $DebugMode ( @DebugModes )
	{
		foreach $ArchName ( @Archies )
		{
			$ReplaceString = $OriginalReplacesString;
			$ReplaceString =~ s/(, )?omiscid-$DaemonName$DebugMode\(= $CurrentVersion\)//;
			$ReplaceString =~ s/^[, ]+//;
			
			print STDERR "Generate omiscid-$DaemonName$DebugMode($CurrentVersion) for $ArchName\n";
			system( "perl CreateDebianPackage.pl $ArchName zeroconf=$DaemonName $DebugMode \"replaces=$ReplaceString\"");
		}
	}
}