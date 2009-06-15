sub WakeOnLan()
{
	my $TestComputer = shift @_;		
	my $MacAddress;
	my $res;
	my $NbTry;
		
	if ( !defined $Configs{$TestComputer} )
	{
		return 0;
	}
	
	$MacAddress = $Configs{$TestComputer};
	
	$MacAddress =~ s/://g;
	`./wol $MacAddress`;
	$NbTry = 5;
	while( $NbTry > 0 )
	{
		$res = `ssh $Computer{$TestComputer} "echo 'ssh is now ok.'"`;
		if ( $res =~ /^ssh is now ok\./ )
		{
			return 1;
		}
		sleep( 5 );
		print STDERR "Send an etherwake command to $TestComputer.\n";
		`./wol $MacAddress`;
		$NbTry--;
	}
	return 0;
}

sub StartAndWaitVM()
{
	my $TestComputer = shift @_;		
	my $res;
	my $NbTry;
	
	# ask VirtualBox to start current VM
	print STDERR "VBoxManage startvm '$TestComputer' ('$Computer{$TestComputer}')\n";
	`VBoxManage startvm $TestComputer`;
	# $res = `perl TestVM.pl`;
	
	# print "$res\n";
	
	# Wait 30 seconds
	sleep(30);

	$NbTry = 20;
	while( $NbTry > 0 )
	{
		$res = `ssh $Computer{$TestComputer} "echo 'ssh is now ok.'"`;
		if ( $res =~ /^ssh is now ok\./ )
		{
			return 1;
		}
		sleep( 5 );
		print STDERR "Retry to connect to $Computer{$TestComputer}.\n";
		$NbTry--;
	}
	return 0;
}

sub StopVM()
{
	my $TestComputer = shift @_;		
	my $res;
	
	# ask VirtualBox to start current VM
	`VBoxManage controlvm $TestComputer acpipowerbutton`;
	
	# Wait 30 seconds, we do not want to have 2 running VMs
	print STDERR "Wait 30 seconds for VM to stop (we do not want 2 VMs at the same time\n";
	sleep(30);
}

sub StartComputer()
{
	my $TestComputer = shift @_;
	
	print STDERR "Trying to start $TestComputer.\n";
	&AddLog( "TRY: Trying to start $TestComputer." );	
	if ( $Configs{$TestComputer} =~ /^0$/ )
	{
		# Start corresponding VM
		&StartAndWaitVM( $TestComputer );
	}
	else
	{
		# MacAdress WakeOnLan
		&WakeOnLan( $TestComputer );
	}
}

sub StopComputer()
{
	my $TestComputer = shift @_;
	if ( $Configs{$TestComputer} =~ /^0$/ )
	{
		# Start corresponding VM
		&StopVM( $TestComputer );
	}
	else
	{
		# MacAdress WakeOnLan
		# nothing to do
	}
}

$Configs{'debian-i386-with-avahi'} = 0;
$Computer{'debian-i386-with-avahi'} = 'rhea';
$Options{'debian-i386-with-avahi'} = '("zeroconf=avahi")';
$SupportedDebugMode{'rhea-i386-with-avahi'} = 'insure';

$Configs{'debian-i386-with-mdns'} = 0;
$Computer{'debian-i386-with-mdns'} = 'rhea';
$Options{'debian-i386-with-mdns'} = '("zeroconf=mdns")';

$Configs{'debian-amd64-with-avahi'} = 0;
$Computer{'debian-amd64-with-avahi'} = 'rhea';
$Options{'debian-amd64-with-avahi'} = '("zeroconf=avahi")';
$SupportedDebugMode{'rhea-amd64-with-avahi'} = 'insure';

$Configs{'debian-amd64-with-mdns'} = 0;
$Computer{'debian-amd64-with-mdns'} = 'rhea';
$Options{'debian-amd64-with-mdns'} = '("zeroconf=mdns")';

$Configs{'MaxOsX'} = '000d936fc38c';
$Computer{'MaxOsX'} = 'metis';
$Options{'MaxOsX'} = '("")';

1;