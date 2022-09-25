#include "shape.h"

using namespace Ray;

//////////////////////
// GlobalProperties //
//////////////////////
bool GlobalProperties::DebugFlag = false;

using namespace Util;

//////////////////////
// ShapeBoundingBox //
//////////////////////
BoundingBox1D ShapeBoundingBox::intersect( const Ray3D &ray ) const
{
	RayTracingStats::IncrementRayBoundingBoxIntersectionNum();
	return Util::BoundingBox3D::intersect( ray );
}

///////////
// Shape //
///////////
unsigned int Shape::OpenGLTessellationComplexity;

unsigned int Shape::WriteInsetSize = 0;

void Shape::WriteInset( std::ostream &stream ){ for( unsigned int i=0 ; i<WriteInsetSize ; i++ ) stream << "  "; }

ShapeBoundingBox Shape::boundingBox( void ) const { return _bBox; }

size_t Shape::primitiveNum( void ) const { return _primitiveNum; }

Shape::ShapeProcessingInfo::ShapeProcessingInfo( void )
{
	localToGlobal = globalToLocal = Matrix4D::Identity();
	directionGlobalToLocal = normalLocalToGlobal = Matrix3D::Identity();
	material = NULL;
}

void Shape::processOverlapping( const Filter &filter , const Kernel &kernel , ShapeProcessingInfo spInfo ) const
{
	if( filter( spInfo , *this )!=ShapeProcessingInfo::NONE ) kernel( spInfo , *this );
}

//////////////////////////
// RayIntersectionStats //
//////////////////////////
std::atomic< size_t > RayTracingStats::_RayNum(0);
std::atomic< size_t > RayTracingStats::_RayPrimitiveIntersectionNum(0);
std::atomic< size_t > RayTracingStats::_RayBoundingBoxIntersectionNum(0);
std::atomic< size_t > RayTracingStats::_ConeBoundingBoxIntersectionNum(0);

void RayTracingStats::Reset( void ){ _RayNum = _RayPrimitiveIntersectionNum = _RayBoundingBoxIntersectionNum = _ConeBoundingBoxIntersectionNum = 0; }
void RayTracingStats::IncrementRayNum( unsigned int count ){ _RayNum += count; }
void RayTracingStats::IncrementRayPrimitiveIntersectionNum( unsigned int count ){ _RayPrimitiveIntersectionNum += count; }
void RayTracingStats::IncrementRayBoundingBoxIntersectionNum( unsigned int count ){ _RayBoundingBoxIntersectionNum += count; }
void RayTracingStats::IncrementConeBoundingBoxIntersectionNum( unsigned int count ){ _ConeBoundingBoxIntersectionNum += count; }

size_t RayTracingStats::RayNum( void ){ return static_cast< size_t >( _RayNum ); }
size_t RayTracingStats::RayPrimitiveIntersectionNum( void ){ return static_cast< size_t >( _RayPrimitiveIntersectionNum ); }
size_t RayTracingStats::RayBoundingBoxIntersectionNum( void ){ return static_cast< size_t >( _RayBoundingBoxIntersectionNum ); }
size_t RayTracingStats::ConeBoundingBoxIntersectionNum( void ){ return static_cast< size_t >( _ConeBoundingBoxIntersectionNum ); }
