#ifndef FACTORY_INCLUDED
#define FACTORY_INCLUDED

#include <type_traits>

namespace Util
{
	/** This templated class represents a factory for generating objects of type BaseType.
	  * It tracks the objects created and deletes them in the destructor. */
	template< typename BaseType >
	class BaseFactory
	{
		/** The list of BaseType created by the factory */
		std::vector< BaseType * > _baseTypes;

		/** The virtual method creating an object of type BaseType on the heap */
		virtual BaseType *_create( void ) = 0;

	public:
		/** The destructor is responsible for deallocating all the BaseType created */
		virtual ~BaseFactory( void ){ for( int i=0 ; i<_baseTypes.size() ; i++ ) delete _baseTypes[i]; }

		/** The (publicly accessible) method for creating a new object */
		BaseType *create( void )
		{
			BaseType *baseType = _create();
			_baseTypes.push_back( baseType );
			return baseType;
		}

		/** The method for creating a new derived object */
		template< typename DerivedType >
		BaseType *create( void )
		{
			static_assert( std::is_base_of< BaseType , DerivedType >::value , "[ERROR] BaseType must be base of DerivedType" );
			BaseType *baseType = new DerivedType();
			_baseTypes.push_back( baseType );
			return baseType;
		}
	};

	/** This derived template class is a factory for creating derived objects of type DerivedType */
	template< typename BaseType , typename DerivedType >
	class DerivedFactory : public BaseFactory< BaseType >
	{
		static_assert( std::is_base_of< BaseType , DerivedType >::value , "[ERROR] BaseType must be base of DerivedType" );

		/////////////////////////////////////
		// BaseFactory< BaseType > methods //
		/////////////////////////////////////
		BaseType *_create( void ){ return new DerivedType(); }
	};
}
#endif // FACTORY_INCLUDED
