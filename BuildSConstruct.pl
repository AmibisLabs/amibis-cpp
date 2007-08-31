sub PrintSources()
{
	my $Folder = shift @_;
	my $First  = 1;
	my $File;
	
	while( $File = <$Folder/*.cpp> )
	{
		# if ( $File =~ /TrackingMemoryLeaks.cpp$/ )
		# {
		# 	next;
		# }
		
		if ( $First )
		{
			$First = 0;
			print $SconsInit "'$File'";
		}
		else
		{
			print $SconsInit ",'$File'";		
		}
	}
}

sub PrintHeaders()
{
	my $Folder = shift @_;
	my $First  = 1;
	my $File;
	
	while( $File = <$Folder/$Folder/*.h> )
	{
		# if ( $File =~ /TrackingMemoryLeaks.h$/ )
		# {
		# 	next;
		# }
		
		if ( $First )
		{
			$First = 0;
			print $SconsInit "['$File','$Folder']";
		}
		else
		{
			print $SconsInit ",['$File','$Folder']";	
		}
	}
}

sub CreateInitTables()
{
	my @Folders = @_;
	my $Folder;
	my $Premier = 1;

	print $SconsInit "# Layer $Folders[0]\n";
	print $SconsInit "$Folders[0]Headers=[";
	foreach $Folder (@Folders)
	{
		if ( $Premier == 1 )
		{
			$Premier = 0;
		}
		else
		{
			print $SconsInit ",";
		}
		&PrintHeaders( $Folder );
	}
	print $SconsInit "]\n";

	$Premier = 1;
	print $SconsInit "$Folders[0]Sources=[";
	foreach $Folder (@Folders)
	{
		if ( $Premier == 1 )
		{
			$Premier = 0;
		}
		else
		{
			print $SconsInit ",";
		}
		&PrintSources( $Folder );
	}
	print $SconsInit "]\n\n";
}

$OmiscidInitfileShortName = "OmiscidInit";
$OmiscidInitfile = $OmiscidInitfileShortName . '.py';

if ( open( $SconsInit, ">$OmiscidInitfile" ) == 0 )
{
	die "Unable to open '$OmiscidInitfile' to produce output\n" ;
}

print $SconsInit "# Tables of layers files\n\n";
print $SconsInit "import os\n\n";

&CreateInitTables('System');
&CreateInitTables('Com');
&CreateInitTables('ServiceControl');

close( $SconsInit );

exit;

# if ( open( $SConstruct, '>SConstruct' ) == 0 )
# {
# 	close( $SconsInit );
# 	unlink( $OmiscidInitfile );
# 	die "Unable to open 'SConstruct' to produce output\n" ;
# }
# 
# print $SConstruct "import os\n";
# print $SConstruct "from $OmiscidInitfileShortName import *\n";
# print $SConstruct 'from OmiscidScons import *
# env = Environment()
# OmiscidLinuxMacOSInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,[\'xml2\'])
# 
# conf = Configure(env)
# OmiscidCheckLibs(conf,[\'xml2\',\'dns_sd\',\'pthread\']);
# env = conf.Finish()
# 
# # common environment settings
# env.AppendUnique(CXXFLAGS = [\'-g\',\'-Werror\',\'-Wall\',\'-pedantic\',\'-std=c++98\'])
# 
# binToInstall = []
# libToInstall = []
# 
# env_system = env.Copy()
# env_system.Append(CPPPATH=\'System\')
# target_system = env_system.SharedLibrary(
#     target=\'OmiscidSystem\',
#     source=SystemSources
# )
# libToInstall += target_system
# 
# env_com = env.Copy()
# env_com.Append(CPPPATH=[\'System\', \'Com\'])
# env_com.Append(LIBPATH=[\'.\'])
# env_com.Append(LIBS = [\'OmiscidSystem\'])
# target_com = env_com.SharedLibrary(
#     target=\'OmiscidCom\',
#     source=ComSources
# )
# 
# libToInstall += target_com
# 
# env_control = env.Copy()
# env_control.Append(CPPPATH=[\'System\', \'Com\', \'ServiceControl\'])
# env_control.Append(LIBPATH=[\'.\'])
# env_control.Append(LIBS = [\'OmiscidCom\', \'OmiscidSystem\'])
# target_control = env_control.SharedLibrary(
#     target=\'OmiscidControl\',
#     source=ServiceControlSources
# )
# libToInstall += target_control
# 
# 
# env.Depends(target_com, target_system)
# env.Depends(target_control, target_com)
# 
# 
# hToInstall = []
# hToInstall += SystemHeaders
# hToInstall += ComHeaders
# hToInstall += ServiceControlHeaders
# 
# binToInstall += OmiscidDotInFileTarget(env, \'Com/OmiscidCom-config\', OmiscidMapping())
# binToInstall += OmiscidDotInFileTarget(env, \'System/OmiscidSystem-config\', OmiscidMapping())
# binToInstall += OmiscidDotInFileTarget(env, \'ServiceControl/OmiscidControl-config\', OmiscidMapping())
# 
# OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)
# 
# '					; # end of the previous print line
# 
# close( $SConstruct );