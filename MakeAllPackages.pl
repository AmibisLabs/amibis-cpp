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

for $DaemonName ( @DaemonNames )
{
	for $DebugMode ( @DebugModes )
	{
		$ReplaceString = $OriginalReplacesString;
		$ReplaceString =~ s/(, )?omiscid-$DaemonName$DebugMode\(= $CurrentVersion\)//;
		$ReplaceString =~ s/^[, ]+//;
		
		print STDERR "Generate omiscid-$DaemonName$DebugMode($CurrentVersion)\n";
		system( "perl CreateDebianPackage.pl zeroconf=$DaemonName $DebugMode \"replaces=$ReplaceString\"");
	}
}