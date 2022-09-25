namespace Ray
{
	////////////////////
	// LocalSceneData //
	////////////////////
	template< typename ParameterType >
	void LocalSceneData::setKeyFrameEvaluator( void )
	{
		if( keyFrameFile ) keyFrameFile->keyFrameMatrices.setEvaluator< ParameterType >();
	}

	///////////////////
	// SceneGeometry //
	///////////////////
	template< typename ParameterType >
	void SceneGeometry::setKeyFrameEvaluator( void )
	{
		_localData.setKeyFrameEvaluator< ParameterType >();
		for( int i=0 ; i<_localData.files.size() ; i++ ) _localData.files[i].setKeyFrameEvaluator< ParameterType >();
	}
}