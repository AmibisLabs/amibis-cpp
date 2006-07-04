sub PrintSources()
{
	my $Folder = shift @_;
	my $First  = 1;
	my $File;
	
	while( $File = <$Folder/*.cpp> )
	{
		if ( $First )
		{
			$First = 0;
			print $SConstruct "'$File'";
		}
		else
		{
			print $SConstruct ",'$File'";		
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
		if ( $First )
		{
			$First = 0;
			print $SConstruct "['$File','$Folder']";
		}
		else
		{
			print $SConstruct ",['$File','$Folder']";	
		}
	}
}


if ( open( $SConstruct, '>SConstruct' ) == 0 )
{
	die "Unable to open 'SConstruct' to produce output\n" ;
}

print $SConstruct 'import os
from OmiscidScons import *
env = Environment()
OmiscidInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,[\'xml2\'])

conf = Configure(env)
OmiscidCheckLibs(conf,[\'xml2\',\'dns_sd\',\'pthread\']);
env = conf.Finish()

# common environment settings
env.AppendUnique(CXXFLAGS = [\'-g\'])

binToInstall = []
libToInstall = []

env_system = env.Copy()
env_system.Append(CPPPATH=\'System\')
target_system = env_system.SharedLibrary(
    target=\'OmiscidSystem\',
    source=['				; # end of the previous print line

&PrintSources( 'System' );

print $SConstruct ']
)
libToInstall += target_system

env_com = env.Copy()
env_com.Append(CPPPATH=[\'System\', \'Com\'])
env_com.Append(LIBPATH=[\'.\'])
env_com.Append(LIBS = [\'OmiscidSystem\'])
target_com = env_com.SharedLibrary(
    target=\'OmiscidCom\',
    source=['				; # end of the previous print line

&PrintSources( 'Com' );

print $SConstruct ']
)

libToInstall += target_com

env_control = env.Copy()
env_control.Append(CPPPATH=[\'System\', \'Com\', \'ServiceControl\'])
env_control.Append(LIBPATH=[\'.\'])
env_control.Append(LIBS = [\'OmiscidCom\', \'OmiscidSystem\'])
target_control = env_control.SharedLibrary(
    target=\'OmiscidControl\',
    source=['				; # end of the previous print line
    
&PrintSources( 'ServiceControl' );
    
print $SConstruct ']
)
libToInstall += target_control


env.Depends(target_com, target_system)
env.Depends(target_control, target_com)


hToInstall = []
hToInstall += ['			; # end of the previous print line

&PrintHeaders( 'System' );

print $SConstruct ']

hToInstall += ['			; # end of the previous print line

&PrintHeaders( 'Com' );

print $SConstruct ']

hToInstall += ['			; # end of the previous print line

&PrintHeaders( 'ServiceControl' );

print $SConstruct ']

binToInstall += OmiscidDotInFileTarget(env, \'Com/Omiscidcom-config\', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, \'System/Omiscidsystem-config\', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, \'ServiceControl/Omiscidcontrol-config\', OmiscidMapping())

OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

'					; # end of the previous print line

close( $SConstruct );