use Cwd;

$ZeroconfInfo{'avahi'} = 'Avahi (see http://avahi.org/)';
$ZeroconfDepends{'avahi'} = ', libavahi-client-dev';
$ZeroconfInfo{'mdns'} = "mDNSresponder (Bonjour from Apple,\n see http://developer.apple.com/networking/bonjour/download/)";
$ZeroconfDepends{'mdns'} = '';

$OMISCID_PACKAGENAME = 'omiscid';
$OMISCID_DEBUG = '';
$OMISCID_ZEROCONF = '';
$PackageSuffix = '';

$Archi{32}{'computer'} = 'desdemona';
$Archi{32}{'suffix'} = '_i386';
$Archi{64}{'computer'} = 'carme';
$Archi{64}{'suffix'} = '_amd64';


$PackageArch = 32;		# default 32 bits


$CurrentArg = 0;
while( defined $ARGV[$CurrentArg] )
{
	if ( $ARGV[$CurrentArg] =~ /^zeroconf\=(.+)$/ )
	{
		if ( $OMISCID_ZEROCONF ne '' )
		{
			die "Multiple definition of zeroconf argument\n";
		}
		$OMISCID_ZEROCONF = $1;
		if ( !defined $ZeroconfInfo{$OMISCID_ZEROCONF} )
		{
			die "Bad zeroconf parameter. Must be mdns or avahi.\n";
		}
		$CurrentArg++;
		next;
	}
	if ( $ARGV[$CurrentArg] =~ /^replaces=(.+)$/ )
	{
		$OMISCID_REPLACES = $1;
		$CurrentArg++;
		next;
	}
	if ( $ARGV[$CurrentArg] eq '-debug' )
	{
		$PackageSuffix = '-debug';
		$OMISCID_DEBUG = 'debug=1';
		$CurrentArg++;
		next;
	}
	if ( $ARGV[$CurrentArg] =~ /^-arch=(.+)$/ )
	{
		$TmpParam = $1;
		
		if ( $TmpParam =~ /^(\d+)bits/ )
		{
			$PackageArch = $1;
			if ( $PackageArch == 32 || $PackageArch == 64 )
			{
				print STDERR "Creating package for $PackageArch architecture.\n";
				$CurrentArg++;
				next;
			}
		}
		die "Unknown package arch for '$TmpParam' for option -arch\n";
		next;
	}
	die "Unknown or bad parameter '$ARGV[$CurrentArg]'\n";
}

$ErrorInArg = 0;
if ( $OMISCID_ZEROCONF eq '' )
{
	print STDERR "Missing zeroconf argument\n";
	$ErrorInArg = 1;
}

if ( $ErrorInArg == 1 )
{
	exit(-1);
}

$OMISCID_ZEROCONF_INFO = $ZeroconfInfo{$OMISCID_ZEROCONF};
$OMISCID_DEPENDANCY = $ZeroconfDepends{$OMISCID_ZEROCONF};

$OMISCID_PACKAGENAME .= '-' . $OMISCID_ZEROCONF . $PackageSuffix;

# search for changelog
$CurrentVersion = '';

open( $fd, '<CHANGES' ) or die "unable to open 'CHANGES' file\n";
# seek first line with a sequence number
while( $line = <$fd> )
{
	if ( $line =~ /^(\d+)\.(\d+)\.(\d+)\s+/ )
	{
		$Major = $1;
		$Middle = $2;
		$Minor = $3;
		$OMISCID_PACKAGEVERSION = "$Major.$Middle.$Minor";
		$OMISCID_MAJORVERSION = "$Major.$Middle.$Minor";
		$OMISCID_SHORTVERSION = $Major.$Middle;
		$VersionCourante = "$Major.$Middle.$Minor";
		$Middle++;
		$OMISCID_NEXTVERSION = "$Major.$Middle";
		$Middle--;
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
	print STDERR "Empty Temp/debian folder\n";
	`rm -rf debian/*`;
}
else
{
	print STDERR "Create Temp/debian folder\n";
	mkdir 'debian', 0755;
}

# parse license file to collect Authors and license
$OMISCID_COPYRIGHT = '';
$OMISCID_AUTHORS = '';
$OMISCID_LICENSE = '';
open( $fd, '<../OMiSCID/LICENCE' ) or die "Unable to open 'LICENCE' file\n";
$OMISCID_COPYRIGHT = <$fd>;
$OMISCID_COPYRIGHT =~ s/[\s\r\n\.]+$//;
$OMISCID_COPYRIGHT =~ s/^Copyright\s+\(c\)\s+//;

while( $line = <$fd> )
{
	if ( $line =~ /^O3MiSCID \(aka OMiSCID\) Software written by\s([^\r\n]+)/ ) #
	{
		$TmpAuthors = $Major.$Middle;
		$TmpAuthors =~ s/^\s+//;
		$TmpAuthors =~ s/[\s\.]+$//;
		$TmpAuthors =~ s/\s+/ /;
		$TmpAuthors =~ s/((\s+)?,|\s+and)(\s+)/ <omiscid\@inrialpes.fr>\r\n    /g;
		$TmpAuthors = '    ' . $TmpAuthors . " <omiscid\@inrialpes.fr>\r\n";
		$OMISCID_AUTHORS = $TmpAuthors;
		while( $line = <$fd> )
		{
			if ( $line =~ /^Previous contributors :/ )
			{
				# we have all the authors
				last;
			}
			$TmpAuthors = $line;
			$TmpAuthors =~ s/^\s+//;
			$TmpAuthors =~ s/[\s\r\n\.]+$//;
			$TmpAuthors =~ s/\s+/ /;
			$TmpAuthors =~ s/((\s+)?,|\s+and)(\s+)/ <omiscid\@inrialpes.fr>\r\n    /g;
			$TmpAuthors = '    ' . $TmpAuthors . " <omiscid\@inrialpes.fr>\r\n";
			$OMISCID_AUTHORS .= $TmpAuthors;
		}
		last;
	}
}
# skip blank lines
while( $line = <$fd> )
{
	next if ( $line =~ /^[\s+\r\n]$/ );
	
	# if not blank, add it to the LICENSE
	$OMISCID_LICENCE = "    " . $line;
	last;
}
# read until end of LICENCE file
while( $line = <$fd> )
{
	$OMISCID_LICENCE .= "    " . $line;
}
close( $fd );

$OMISCID_LICENCE =~ s/[\r\n]+/\r\n/gm;

# die "Authors : $OMISCID_AUTHORS\nLICENCE :\n$OMISCID_LICENCE\n";

foreach $File ( <$WorkingRep/debian-param/*> )
{
	print STDERR "Process $File to ";
	
	# Get only file name
	$File =~ /\/([^\/]+)$/;
	$ShortFile = $1;
	
	if ( $ShortFile =~ /^omiscid-dev.install$/ )
	{
		# die "cp $File 'debian/omiscid-dev.install'\n";
		`cp $File 'debian/omiscid-dev.install'`;
		next;
	}
	
	if ( $ShortFile =~ /^omiscid.install$/ )
	{
		`cp $File 'debian/$OMISCID_PACKAGENAME.install'`;
		# die "cp $File 'debian/$OMISCID_PACKAGENAME.install'\n";
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
		$line =~ s/\$OMISCID_SHORTVERSION/$OMISCID_SHORTVERSION/g;
		$line =~ s/\$OMISCID_CHANGELOG/$OMISCID_CHANGELOG/g;
		$line =~ s/\$OMISCID_PACKAGEDATE/$OMISCID_PACKAGEDATE/g;
		$line =~ s/\$OMISCID_DEBUG/$OMISCID_DEBUG/g;
		$line =~ s/\$OMISCID_LICENCE/$OMISCID_LICENCE/g;
		$line =~ s/\$OMISCID_AUTHORS/$OMISCID_AUTHORS/g;
		$line =~ s/\$OMISCID_COPYRIGHT/$OMISCID_COPYRIGHT/g;
		$line =~ s/\$OMISCID_REPLACES/$OMISCID_REPLACES/g;
		$line =~ s/\$OMISCID_NEXTVERSION/$OMISCID_NEXTVERSION/g;
		
		$content .= $line;
	}
	
	# if ( $content =~ /\$/m )
	# {
	# 	die "Missing variable for :\n$content\n";
	# }
	close( $fd );
	
	print STDERR "debian/$ShortFile\n";
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

$computer = $Archi{$PackageArch}{'computer'};

if ( $OMISCID_ZEROCONF eq 'mdns' )
{
	system( "ssh $computer \"cd tmp/$PackageFolder; debuild -us -uc\"" );
	# die "ssh $computer \"cd tmp/$PackageFolder; dpkg-buildpackage\"\n";
	# system( "ssh $computer \"cd tmp/$PackageFolder; dpkg-buildpackage\"" );
	chdir('..');
	print "scp oberon:tmp/${OMISCID_PACKAGENAME}_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb oberon:tmp/${OMISCID_PACKAGENAME}-dev_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb .\n";
	`scp oberon:tmp/${OMISCID_PACKAGENAME}_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb oberon:tmp/omiscid-dev_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb .`;
}
else
{
 	system( "ssh $computer \"cd tmp/$PackageFolder; sudo pbuilder create --distribution lenny; sudo pbuilder update; pdebuild\" ");
 	chdir('..');
 	print "scp $computer:/var/cache/pbuilder/result/${OMISCID_PACKAGENAME}_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb $computer:/var/cache/pbuilder/result/${OMISCID_PACKAGENAME}-dev_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb .\n";
 	`scp $computer:/var/cache/pbuilder/result/${OMISCID_PACKAGENAME}_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb $computer:/var/cache/pbuilder/result/omiscid-dev_${OMISCID_MAJORVERSION}$Archi{$PackageArch}{'suffix'}.deb .`;
}

print STDERR "Remove OMiSCID folder in tmp on oberon\n";
# `ssh oberon "rm -rf tmp/omiscid*"`;

print STDERR "Remove OMiSCID folder in Temp\n";
# `rm -rf OMiSCID`;

