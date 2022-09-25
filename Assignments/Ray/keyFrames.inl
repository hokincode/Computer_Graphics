namespace Ray
{
	///////////////////////
	// KeyFrameEvaluator //
	///////////////////////
	template< typename DataType >
	KeyFrameEvaluator< DataType >::~KeyFrameEvaluator( void ){}

	//////////////////
	// KeyFrameData //
	//////////////////
	template< typename DataType >
	KeyFrameData< DataType >::KeyFrameData( void ) : _keyFrameEvaluator(NULL) {}

	template< typename DataType >
	KeyFrameData< DataType >::~KeyFrameData( void ) { if( _keyFrameEvaluator ) delete _keyFrameEvaluator; }

	template< typename DataType >
	float KeyFrameData< DataType >::duration( void ) const { return _duration; }

	template< typename DataType >
	int KeyFrameData< DataType >::keyframes( void ) const { return (int)_data[0].size(); }

	template< typename DataType >
	int KeyFrameData< DataType >::dofs( void ) const { return (int)_data.size(); }

	template< typename DataType >
	const DataType &KeyFrameData< DataType >::current( const std::string &dofName ) const
	{
		for( int i=0 ; i<_dofNames.size() ; i++ ) if( _dofNames[i]==dofName ) return _currentValues[i];
		THROW( "could not find dof name: " , dofName );
		return _currentValues[0];
	}

	template< typename DataType >
	template< typename ParameterType >
	void KeyFrameData< DataType >::setEvaluator( void )
	{
		if( _keyFrameEvaluator ) delete _keyFrameEvaluator;
		_keyFrameEvaluator = new KeyFrameParameters< DataType , ParameterType >( *this );
	}

	template< typename DataType >
	void KeyFrameData< DataType >::setCurrentValues( double t , int curveType )
	{
		if( !_keyFrameEvaluator ) THROW( "_keyFrameEvaluator has not been initialized" );
		for( int dof=0 ; dof<_currentValues.size() ; dof++ ) _currentValues[ dof ] = _keyFrameEvaluator->evaluate( dof , t , curveType );
	}

	template< typename DataType >
	std::ostream &operator << ( std::ostream &stream , const KeyFrameData< DataType > &keyFrameData )
	{
		stream << "#DOFS  " << keyFrameData._dofNames.size() << std::endl;
		for( int i=0 ; i<keyFrameData._dofNames.size() ; i++ ) stream << "  " << keyFrameData._dofNames[i] << std::endl;
		stream << "#DURATION  " << keyFrameData._duration << std::endl;
		stream << "#FRAMES  " << keyFrameData._data[0].size() << std::endl;
		for( int i=0 ; i<keyFrameData._data[0].size() ; i++ ) 
		{
			for( int j=0 ; j<keyFrameData._data.size() ; j++ ) stream << "  " << keyFrameData._data[j][i];
			stream << std::endl;
		}
		return stream;
	}

	template< typename DataType >
	std::istream &operator >> ( std::istream &stream , KeyFrameData< DataType > &keyFrameData )
	{
		std::string str;
		int dofs;
		stream >> str >> dofs;
		if( !stream || str!="#DOFS" || dofs<=0 ) THROW( "Failed to parse DOFS" );

		keyFrameData._currentValues.resize( dofs );
		keyFrameData._dofNames.resize( dofs );
		keyFrameData._data.resize( dofs );

		for( int i=0 ; i<dofs ; i++ ) if( !( stream >> keyFrameData._dofNames[i] ) ) THROW( "Failed to read DOF names" );

		stream >> str >> keyFrameData._duration;
		if( !stream || str!="#DURATION" || keyFrameData._duration<=0 ) THROW( "Failed to read DURATION" );

		int frames;
		stream >> str >> frames;
		if( !stream || str!="#FRAMES" || frames<=0 ) THROW( "Failed to read FRAMES" );

		for( int i=0 ; i<keyFrameData._data.size() ; i++ ) keyFrameData._data[i].resize( frames );

		for( int j=0 ; j<frames ; j++ ) for( int i=0 ; i<dofs ; i++ ) if( !( stream >> keyFrameData._data[i][j] ) ) THROW( "Failed to read parameter" );
		return stream;
	}

	////////////////////////
	// KeyFrameParameters //
	////////////////////////
	template< typename DataType , typename ParameterType >
	KeyFrameParameters< DataType , ParameterType >::KeyFrameParameters( const KeyFrameData< DataType > &keyFrameData )
	{
		// Allocate for the parameters
		_parameters.resize( keyFrameData._data.size() );
		for( int i=0 ; i<_parameters.size() ; i++ ) _parameters[i].resize( keyFrameData._data[i].size() );

		// Transform data -> parameters
		for( int d=0 ; d<_parameters.size() ; d++ )
		{
			// Set the first transformation naively
			_parameters[d][0] = ParameterType( keyFrameData._data[d][0] );
			// Set the rest using the current and previous
			for( int f=1 ; f<_parameters[d].size() ; f++ ) _parameters[d][f] = ParameterType( keyFrameData._data[d][f] , _parameters[d][f-1] );
		}
	}

	template< typename DataType , typename ParameterType >
	DataType KeyFrameParameters< DataType , ParameterType >::evaluate( unsigned int dof , double t , int curveType )
	{
		ParameterType param = Util::Interpolation::Sample( _parameters[dof] , t , curveType );
		return param();
	}
}