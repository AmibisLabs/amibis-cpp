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
			print $SConstruct "\"$File\"";
		}
		else
		{
			print $SConstruct ",\"$File\"";		
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
			print $SConstruct "[\"$File\",\"$Folder\"]";
		}
		else
		{
			print $SConstruct ",[\"$File\",\"$Folder\"]";	
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
primaInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,["xml2"])

conf = Configure(env)
primaCheckLibs(conf,["xml2","dns_sd","pthread"]);
env = conf.Finish()


binToInstall = []
libToInstall = []

env.Append(CPPPATH="System")
libToInstall += env.SharedLibrary(
    target="primasystem",
    source=['				; # end of the previous print line

&PrintSources( 'System' );

print $SConstruct '],
    libs=["pthread"]
)

env.Append(CPPPATH="Com")
libToInstall += env.SharedLibrary(
    target="com",
    source=['				; # end of the previous print line

&PrintSources( 'Com' );

print $SConstruct ']
)

env.Append(CPPPATH="ServiceControl")
libToInstall += env.SharedLibrary(
    target="control",
    source=['				; # end of the previous print line
    
&PrintSources( 'ServiceControl' );
    
print $SConstruct ']
)

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

binToInstall += primaDotInFileTarget(env, "Com/primacom-config", primaMapping())
binToInstall += primaDotInFileTarget(env, "System/primasystem-config", primaMapping())
binToInstall += primaDotInFileTarget(env, "ServiceControl/primacontrol-config", primaMapping())

primaInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

'					; # end of the previous print line

close( $SConstruct );