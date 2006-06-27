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
from primaScons import *
env = Environment()
primaInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,[\'xml2\'])

conf = Configure(env)
primaCheckLibs(conf,[\'xml2\',\'dns_sd\',\'pthread\']);
env = conf.Finish()

# common environment settings
env.AppendUnique(CXXFLAGS = [\'-g\'])

binToInstall = []
libToInstall = []

env_system = env.Copy()
env_system.Append(CPPPATH=\'System\')
target_system = env_system.SharedLibrary(
    target=\'primasystem\',
    source=['				; # end of the previous print line

&PrintSources( 'System' );

print $SConstruct ']
)
libToInstall += target_system

env_com = env.Copy()
env_com.Append(CPPPATH=[\'System\', \'Com\'])
env_com.Append(LIBPATH=[\'.\'])
env_com.Append(LIBS = [\'primasystem\'])
target_com = env_com.SharedLibrary(
    target=\'com\',
    source=['				; # end of the previous print line

&PrintSources( 'Com' );

print $SConstruct ']
)

libToInstall += target_com

env_control = env.Copy()
env_control.Append(CPPPATH=[\'System\', \'Com\', \'ServiceControl\'])
env_control.Append(LIBPATH=[\'.\'])
env_control.Append(LIBS = [\'com\', \'primasystem\'])
target_control = env_control.SharedLibrary(
    target=\'control\',
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

binToInstall += primaDotInFileTarget(env, \'Com/primacom-config\', primaMapping())
binToInstall += primaDotInFileTarget(env, \'System/primasystem-config\', primaMapping())
binToInstall += primaDotInFileTarget(env, \'ServiceControl/primacontrol-config\', primaMapping())

primaInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

'					; # end of the previous print line

close( $SConstruct );