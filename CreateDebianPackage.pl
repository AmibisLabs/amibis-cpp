use Cwd;

$ZeroconfInfo{'avahi'} = 'Avahi (see http://avahi.org/)';
$ZeroconfDepends{'avahi'} = ', libavahi-client-dev';
$ZeroconfInfo{'mdns'} = "mDNSresponder (Bonjour from Apple,\n see http://developer.apple.com/networking/bonjour/download/)";
$ZeroconfDepends{'mdns'} = '';


$OMISCID_PACKAGENAME = 'omiscid';
$OMISCID_DEBUG = '';

if ( !defined $ARGV[0] )
{
	die "Missing zeroconf parameter\n";
}

$OMISCID_ZEROCONF = $ARGV[0];

if ( !defined $ZeroconfInfo{$OMISCID_ZEROCONF} )
{
	die "Bad zeroconf parameter. Must be mdns or avahi.\n";
}

$PackageSuffix = '';
if ( defined $ARGV[1] )
{
	if ( $ARGV[1] ne '-deb' )
	{
		die "Bad '$ARGV[1]' parameter. Must be '-deb'.\n";
	}
	$PackageSuffix = '-deb';
	$OMISCID_DEBUG = 'debug=1';
}

$OMISCID_ZEROCONF_INFO = $ZeroconfInfo{$OMISCID_ZEROCONF};
$OMISCID_DEPENDANCY = $ZeroconfDepends{$OMISCID_ZEROCONF};

$OMISCID_PACKAGENAME .= '-using-' . $OMISCID_ZEROCONF . $PackageSuffix;

# search for changelog
$CurrentVersion = '';

open( $fd, '<CHANGES' );
# seek first line with a sequence number
while( $line = <$fd> )
{
	if ( $line =~ /^(\d+\.\d+)\.(\d+)\s+/ )
	{
		$OMISCID_PACKAGEVERSION = "$1.$2";
		$OMISCID_MAJORVERSION = "$1.$2";
		$VersionCourante = "$1.$2";
		last;
	}
}

# after here, line are in change log
$OMISCID_CHANGELOG = '';
while( $line = <$fd> )
{
	if ( $line =~ /^\d+\.\d+\.\d+\s+/ )
	{
		last;
	}
	$OMISCID_CHANGELOG .= $line;
}
close( $fd );

# modify current version to be compliant with
# assign the returned list elements to scalars for ease of use
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

# change value to be correct
$year += 1900;
$hour = sprintf( "%2.2d:%2.2d:%2.2d", $hour, $min, $sec );
if ( $isdst >= 0 )
{
	$isdst = sprintf( "+%2.2d00", $isdst );
}
else
{
	$isdst = sprintf( "%2.2d00", $isdst );
}

my @weekdays = qw(Sun Mond Tue Wed Thu Fri Sat);

my @months = ( "Jan", "Feb", "Mar", "Apr",
                   "May", "Jun", "Jul", "Aug", "Sep",
                   "Oct", "Nov", "Dec" );

# generate data like : Tue, 25 Mar 2008 16:51:23 +0100                   
$OMISCID_PACKAGEDATE = "$weekdays[$wday], $mday $months[$mon] $year $hour $isdst";

$WorkingRep = cwd;

if ( !-e '../Temp' )
{
	print STDERR "Create Temp folder\n";
	mkdir '../Temp', 0755;
}

chdir('../Temp');
if ( -e debian )
{
	print STDER "Empty Temp/debian folder\n";
	`rm -rf debian/*`;
}
else
{
	print STDER "Create Temp/debian folder\n";
	mkdir 'debian', 0755;
}

foreach $File ( <$WorkingRep/debian-param/*> )
{
	print STDERR "Process $File\n";
	
	# Get only file name
	$File =~ /\/([^\/]+)$/;
	$ShortFile = $1;
	
	if ( $ShortFile =~ /^omiscid(.*).install$/ )
	{
		# die "mv $File debian/$OMISCID_PACKAGENAME$1.install\n";
		`mv $File 'debian/$OMISCID_PACKAGENAME$1.install'`;
		next;
	}
	
	$content = '';
	open( $fd, $File );
	while( $line = <$fd> )
	{
		$line =~ s/\$OMISCID_PACKAGENAME/$OMISCID_PACKAGENAME/g;
		$line =~ s/\$OMISCID_ZEROCONF/$OMISCID_ZEROCONF/g;
		$line =~ s/\$OMISCID_ZEROCONF_INFO/$OMISCID_ZEROCONF_INFO/g;
		$line =~ s/\$OMISCID_DEPENDANCY/$OMISCID_DEPENDANCY/g;
		$line =~ s/\$OMISCID_PACKAGEVERSION/$OMISCID_PACKAGEVERSION/g;
		$line =~ s/\$OMISCID_MAJORVERSION/$OMISCID_MAJORVERSION/g;
		$line =~ s/\$OMISCID_CHANGELOG/$OMISCID_CHANGELOG/g;
		$line =~ s/\$OMISCID_PACKAGEDATE/$OMISCID_PACKAGEDATE/g;
		$line =~ s/\$OMISCID_DEBUG/$OMISCID_DEBUG/g;
		$content .= $line;
	}
	
	# if ( $content =~ /\$/m )
	# {
	# 	die "Missing variable for :\n$content\n";
	# }
	close( $fd );
	

	
	open( $fd, ">debian/$ShortFile" );
	print $fd $content;
	close( $fd );
}

if ( -e 'OMiSCID' )
{
	print STDERR "Remove old OMiSCID folder in Temp\n";
	`rm -rf OMiSCID`;
}

print STDERR "Unzip current OMiSCID version to Temp\n";
`unzip ../Omiscid-$VersionCourante.zip`;

print STDERR "Move debian folder in OMiSCID\n";
`mv debian OMiSCID/`;

$PackageFolder = $OMISCID_PACKAGENAME . '-' . $OMISCID_MAJORVERSION;

print STDERR "Remove oberon:tmp/$PackageFolder if any\n";
`ssh oberon "rm -rf tmp/$PackageFolder"`;

print STDERR "Copy Temp/OMiSCID to oberon:tmp/$PackageFolder\n";
`scp -r  OMiSCID oberon:tmp/$PackageFolder`;

print STDERR "Change permission in tmp/$PackageFolder\n";
`ssh oberon "cd tmp; chmod -R 755 $PackageFolder; chmod 777 $PackageFolder/debian/rules"`;

print STDERR "Create tmp/$PackageFolder.orig\n";
`ssh oberon "cd tmp; rm -rf $PackageFolder.orig; cp -r $PackageFolder $PackageFolder.orig"`;

if ( $OMISCID_ZEROCONF eq 'mdns' )
{
	system( "ssh carme \"cd tmp/$PackageFolder; debuild -us -uc\"" );
}
else
{
	system( "ssh carme \"cd tmp/$PackageFolder; sudo pbuilder create --distribution lenny; sudo pbuilder update; pdebuild\" ");
}

print STDERR "Remove OMiSCID folder in Temp\n";
# `rm -rf OMiSCID`;

