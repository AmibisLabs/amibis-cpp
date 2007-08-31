
	DateAndTime DET;

	fprintf( stderr, "%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d:%3.3d\n", DET.Day, DET.Month, DET.Year,
						DET.Hours, DET.Minutes, DET.Seconds, DET.Milliseconds );


SimpleList<SimpleString> Tests;
	SimpleList<SimpleString> Results;

	SimpleString Test, Res, Tmp;
	bool Modified;
	int NbErrors = 0;

	Tests.Add( "www.google.fr" );
	Results.Add( "www.google.fr" );
	Tests.Add( "www.google2.fr" );
	Results.Add( "www.google2.fr" );
	Tests.Add( "www.goo-g2le.fr" );
	Results.Add( "www.goo-g2le.fr" );
	Tests.Add( "www.google-5.fr" );
	Results.Add( "www.google.fr" );
	Tests.Add( "www.go-52ogle.fr" );
	Results.Add( "www.google.fr" );
	Tests.Add( "www.google-514.fr" );
	Results.Add( "www.google.fr" );
	Tests.Add( "www-61531357537357357.google.fr" );
	Results.Add( "www.google.fr" );

	while( Tests.IsNotEmpty() )
	{
		Test = Tests.ExtractFirst();
		Res  = Results.ExtractFirst();

		// Compute test
		Modified = false;
		Tmp = Socket::RemoveDnsSdDaemonNumber( Test, Modified );
		if ( Tmp != Res )
		{
			OmiscidError( "Test failed on '%s' (got '%s' instead of '%s')\n", Test.GetStr(), Tmp.GetStr(), Res.GetStr() );
			NbErrors++;
		}
	}

	if ( NbErrors == 0 )
	{
		OmiscidError( "Tests ok\n" );
	}
	else
	{
		OmiscidError( "Tests failed with %d error(s).\n", NbErrors );
	}