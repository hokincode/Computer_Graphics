#ifndef KEY_FILE_INCLUDED
#define KEY_FILE_INCLUDED

#include <vector>
#include <string>
#include <Util/geometry.h>
#include <Util/interpolation.h>

namespace Ray
{
	/** This class represents a black box that can be used to evaluate the dofs */
	template< typename DataType >
	class KeyFrameEvaluator
	{
	public:
		/** The destructor */
		virtual ~KeyFrameEvaluator( void );

		/** This method interpolates the parameters associated to the prescribed degree of freedom and returns the associated data */
		virtual DataType evaluate( unsigned int dof , double t , int curveType ) = 0;
	};

	/** This templated class represents a set of key-frame values associated to named degrees of freedom */
	template< typename DataType >
	class KeyFrameData
	{
		template< typename _DataType > friend std::ostream &operator << ( std::ostream & , const KeyFrameData<_DataType> & );
		template< typename _DataType > friend std::istream &operator >> ( std::istream & ,       KeyFrameData<_DataType> & );
		template< typename _DataType , typename ParameterType > friend class KeyFrameParameters;

		/** The duration of the animation */
		float _duration;

		/** The names of the degrees of freedom */
		std::vector< std::string > _dofNames;

		/** The key-frame values for the different degrees of freedom */
		std::vector< std::vector< DataType > > _data;

		/** The current values for the different degrees of freedom */
		std::vector< DataType > _currentValues;

		/** An object enabling the interpolation of key frame values */
		KeyFrameEvaluator< DataType > *_keyFrameEvaluator;
	public:
		/** The default constructor */
		KeyFrameData( void );

		/** The destructor */
		~KeyFrameData( void );

		/** This is the duration (in seconds) over which the animation is to play */
		float duration( void ) const;

		/** This method returns the number of key-frames stored. */
		int keyframes( void ) const;

		/** This method returns the number of parameters stored. */
		int dofs( void ) const;

		/** This method returns a reference to the DataType storing the current data for the specified dof  */
		const DataType &current( const std::string &dofName ) const;

		/** This templated method sets the evaluator using the prescribed type of parameter */
		template< typename ParameterType >
		void setEvaluator( void );

		/** This method updates the current value of all the parameters, using the interpolation/approximation method specified by curveType */
		void setCurrentValues( double t , int curveType );
	};

	/** This operator writes the key-frame data out to a stream.*/
	template< typename DataType > std::ostream &operator << ( std::ostream &stream , const KeyFrameData< DataType > &keyFrameData );

	/** This operator reads the key-frame data in from a stream.*/
	template< typename DataType > std::istream &operator >> ( std::istream &stream ,       KeyFrameData< DataType > &keyFrameData );

	/** This class stores the key-frame transformation as 4x4 matrices */
	typedef KeyFrameData< Util::Matrix4D > KeyFrameMatrices;

	/** This class represents a set of key-frame parameters
	* Assumes that ParameterType defines constructors of the form:
	*    ParameterType( DataType )
	* and
	*    ParameterType( DataType , ParameterType )
	* as well as an operator of the form:
	*    DataType operator() ( void ) const;
	*/
	template< typename DataType , typename ParameterType >
	class KeyFrameParameters : public KeyFrameEvaluator< DataType >
	{
		/** The parameters for the key-frame values of the different degrees of freedom */
		std::vector< std::vector< ParameterType > > _parameters;
	public:
		/** This constructor creates a set of parameters from the input data */
		KeyFrameParameters( const KeyFrameData< DataType > &data );

		///////////////////////////////
		// KeyFrameEvaluator methods //
		///////////////////////////////
		DataType evaluate( unsigned int dof , double t , int curveType );
	};
}
#include "keyFrames.inl"
#endif // KEY_FILE_INCLUDED
