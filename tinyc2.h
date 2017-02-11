#if !defined( TINYC2_H )

// this can be adjusted as necessary, but is highly recommended to be kept at 8.
// higher numbers will incur quite a bit of memory overhead, and convex shapes
// over 8 verts start to just look like spheres, which can be implicitly rep-
// resented as a point + radius. usually tools that generate polygons should be
// constructed so they do not output polygons with too many verts.
// Note: polygons in tinyc2 are all *convex*.
#define C2_MAX_POLYGON_VERTS 8

// 2d vector
typedef struct
{
	float x;
	float y;
} c2v;

// 2d rotation composed of cos/sin pair
typedef struct
{
	float c;
	float s;
} c2r;

// 2d rotation matrix
typedef struct
{
	c2v x;
	c2v y;
} c2m;

// 2d transformation "x"
typedef struct
{
	c2v p;
	c2r r;
} c2x;

// 2d halfspace (aka plane, aka line)
typedef struct
{
	c2v n;   // normal, normalized
	float d; // distance to origin from plane, or ax + by = d
} c2h;

typedef struct
{
	c2v p;
	float r;
} c2Circle;

typedef struct
{
	c2v min;
	c2v max;
} c2AABB;

// a capsule is defined as a line segment (from a to b) and radius r
typedef struct
{
	c2v a;
	c2v b;
	float r;
} c2Capsule;

typedef struct
{
	int count;
	c2v verts[ C2_MAX_POLYGON_VERTS ];
	c2v norms[ C2_MAX_POLYGON_VERTS ];
} c2Poly;

typedef struct
{
	c2v p;   // position
	c2v d;   // direction (normalized)
	float t; // distance along d from position p to find endpoint of ray
} c2Ray;

typedef struct
{
	float t; // time of impact
	c2v p;   // position of impact p = ray.p + ray.d * raycast.t
} c2Raycast;

// contains all information necessary to resolve a collision, or in other words
// this is the information needed to separate shapes that are colliding. Doing
// the resolution step is *not* included in tinyc2
typedef struct
{
	int count;
	float depths[ 2 ];
	c2v contact_points[ 2 ];
	c2v normal;
} c2Manifold;

// boolean collision detection
// these versions are faster than the manifold versions, but only give a YES/NO
// result
int c2CircletoCircle( c2Circle A, c2Circle B );
int c2CircletoAABB( c2Circle A, c2AABB B );
int c2CircletoCapsule( c2Circle A, c2Capsule B );
int c2AABBtoAABB( c2AABB A, c2AABB B );
int c2AABBtoCapsule( c2AABB A, c2Capsule B );
int c2CapsuletoCapsule( c2Capsule A, c2Capsule B );
int c2CircletoPoly( c2Circle A, c2Poly* B, c2x* bx );
int c2AABBtoPoly( c2AABB A, c2Poly* B, c2x* bx );
int c2CapsuletoPoly( c2Capsule A, c2Poly* B, c2x* bx );
int c2PolytoPoly( c2Poly* A, c2x* ax, c2Poly* B, c2x* bx );

// ray operations
// output is placed into the c2Raycast struct, which represents the hit location
// of the ray. the out param contains no meaningful information if these funcs
// return 0
int c2RaytoCircle( c2Ray A, c2Circle B, c2Raycast* out );
int c2RaytoAABB( c2Ray A, c2AABB B, c2Raycast* out );
int c2RaytoCapsule( c2Ray A, c2Capsule B, c2Raycast* out );
int c2RaytoPoly( c2Ray A, c2Poly* B, c2x bx, c2Raycast* out );

// manifold generation
// these functions are slower than the boolean versions, but will compute one
// or two points that represent the plane of contact. This information is
// is usually needed to resolve and prevent shapes from colliding. If no coll
// ision occured the count member of the manifold struct is set to 0.
void c2CircletoCircleManifold( c2Circle A, c2Circle B, c2Manifold* m );
void c2CircletoAABBManifold( c2Circle A, c2AABB B, c2Manifold* m );
void c2CircletoCapsuleManifold( c2Circle A, c2Capsule B, c2Manifold* m );
void c2AABBtoAABBManifold( c2AABB A, c2AABB B, c2Manifold* m );
void c2AABBtoCapsuleManifold( c2AABB A, c2Capsule B, c2Manifold* m );
void c2CapsuletoCapsuleManifold( c2Capsule A, c2Capsule B, c2Manifold* m );
void c2CircletoPolyManifold( c2Circle A, c2Poly* B, c2x* bx, c2Manifold* m );
void c2AABBtoPolyManifold( c2AABB A, c2Poly* B, c2x* bx, c2Manifold* m );
void c2CapsuletoPolyManifold( c2Capsule A, c2Poly* B, c2x* bx, c2Manifold* m );
void c2PolytoPolyManifold( c2Poly* A, c2x* ax, c2Poly* B, c2x* bx, c2Manifold* m );

typedef enum
{
	C2_CIRCLE,
	C2_AABB,
	C2_CAPSULE,
	C2_POLY
} C2_TYPE;

// Runs the GJK algorithm to find closest points, returns distance between closest points.
// outA and outB can be NULL, in this case only distance is returned. ax_ptr and bx_ptr
// can be NULL, and represent local to world transformations for shapes A and B respectively.
// use_radius will apply radii for capsules and circles (if set to false, spheres are
// treated as points and capsules are treated as line segments i.e. rays).
float c2GJK( void* A, C2_TYPE typeA, c2x* ax_ptr, void* B, C2_TYPE typeB, c2x* bx_ptr, c2v* outA, c2v* outB, int use_radius );

// Computes 2D convex hull. Will not do anything if less than two verts supplied. If
// more than C2_MAX_POLYGON_VERTS are supplied extras are ignored.
int c2Hull( c2v* verts, int count );

// Generic collision detection routines, useful for games that want to use some poly-
// morphism to write more generic-styled code. Internally calls various above functions.
int c2Collided( void* A, c2x* ax, C2_TYPE typeA, void* B, c2x* bx, C2_TYPE typeB );
void c2Collide( void* A, c2x* ax, C2_TYPE typeA, void* B, c2x* bx, C2_TYPE typeB, c2Manifold* m );

#ifdef _WIN32
	#define C2_INLINE __forceinline
#else
	#define C2_INLINE __attribute__((always_inline))
#endif

// adjust these primitives as necessary
#include <math.h>
#define c2Sin( radians ) sinf( radians )
#define c2Cos( radians ) cosf( radians )
#define c2Sqrt( a ) sqrtf( a )
#define c2Min( a, b ) ((a) < (b) ? (a) : (b))
#define c2Max( a, b ) ((a) > (b) ? (a) : (b))
#define c2Abs( a ) ((a) < 0 ? -(a) : (a))
#define c2Clamp( a, lo, hi ) c2Max( lo, c2Min( a, hi ) )
C2_INLINE void c2SinCos( float radians, float* s, float* c ) { *c = c2Cos( radians ); *s = c2Sin( radians ); }

// vector ops
C2_INLINE c2v c2V( float x, float y ) { c2v a; a.x = x; a.y = y; return a; }
C2_INLINE c2v c2Add( c2v a, c2v b ) { a.x += b.x; a.y += b.y; return a; }
C2_INLINE c2v c2Sub( c2v a, c2v b ) { a.x -= b.x; a.y -= b.y; return a; }
C2_INLINE float c2Dot( c2v a, c2v b ) { return a.x * b.x + a.y * b.y; }
C2_INLINE c2v c2Mulvs( c2v a, float b ) { a.x *= b; a.y *= b; return a; }
C2_INLINE c2v c2Mulvv( c2v a, c2v b ) { a.x *= b.x; a.y *= b.y; return a; }
C2_INLINE c2v c2Div( c2v a, float b ) { return c2Mulvs( a, 1.0f / b ); }
C2_INLINE c2v c2Skew( c2v a ) { c2v b; b.x = -a.y; b.y = a.x; return b; }
C2_INLINE c2v c2CW90( c2v a ) { c2v b; b.x = a.y; b.y = -a.x; return b; }
C2_INLINE float c2Det2( c2v a, c2v b ) { return a.x * b.y - a.y * b.x; }
C2_INLINE c2v c2Minv( c2v a, c2v b ) { return c2V( c2Min( a.x, b.x ), c2Min( a.y, b.y ) ); }
C2_INLINE c2v c2Maxv( c2v a, c2v b ) { return c2V( c2Max( a.x, b.x ), c2Max( a.y, b.y ) ); }
C2_INLINE c2v c2Clampv( c2v a, c2v lo, c2v hi ) { return c2Maxv( lo, c2Minv( a, hi ) ); }
C2_INLINE c2v c2Absv( c2v a ) { return c2V( c2Abs( a.x ), c2Abs( a.y ) ); }
C2_INLINE float c2Hmin( c2v a ) { return c2Min( a.x, a.y ); }
C2_INLINE float c2Hmax( c2v a ) { return c2Max( a.x, a.y ); }
C2_INLINE float c2Len( c2v a ) { return c2Sqrt( c2Dot( a, a ) ); }
C2_INLINE c2v c2Norm( c2v a ) { return c2Div( a, c2Len( a ) ); }
C2_INLINE c2v c2Neg( c2v a ) { return c2V( -a.x, -a.y ); }
C2_INLINE int c2Parallel( c2v a, c2v b, float kTol )
{
	float k = c2Len( a ) / c2Len( b );
	b = c2Mulvs( b, k );
	if ( c2Abs( a.x - b.x ) < kTol && c2Abs( a.y - b.y ) < kTol ) return 1;
	return 0;
}

// rotation ops
C2_INLINE c2r c2Rot( float radians ) { c2r r; c2SinCos( radians, &r.s, &r.c ); return r; }
C2_INLINE c2r c2RotIdentity( ) { c2r r; r.c = 1.0f; r.s = 0; return r; }
C2_INLINE c2v c2RotX( c2r r ) { return c2V( r.c, r.s ); }
C2_INLINE c2v c2RotY( c2r r ) { return c2V( -r.s, r.c ); }
C2_INLINE c2v c2Mulrv( c2r a, c2v b )  { return c2V( a.c * b.x - a.s * b.y,  a.s * b.x + a.c * b.y ); }
C2_INLINE c2v c2MulrvT( c2r a, c2v b ) { return c2V( a.c * b.x + a.s * b.y, -a.s * b.x + a.c * b.y ); }
C2_INLINE c2r c2Mulrr( c2r a, c2r b )  { c2r c; c.c = a.c * b.c - a.s * b.s; c.s = a.s * b.c + a.c * b.s; return c; }
C2_INLINE c2r c2MulrrT( c2r a, c2r b ) { c2r c; c.c = a.c * b.c + a.s * b.s; c.s = a.c * b.s - a.s * b.c; return c; }

C2_INLINE c2v c2Mulmv( c2m a, c2v b ) { c2v c; c.x = a.x.x * b.x + a.y.x * b.y; c.y = a.x.y * b.x + a.y.y * b.y; return c; }
C2_INLINE c2v c2MulmvT( c2m a, c2v b ) { c2v c; c.x = a.x.x * b.x + a.x.y * b.y; c.y = a.y.x * b.x + a.y.y * b.y; return c; }
C2_INLINE c2m c2Mulmm( c2m a, c2m b )  { c2m c; c.x = c2Mulmv( a, b.x );  c.y = c2Mulmv( a, b.y ); return c; }
C2_INLINE c2m c2MulmmT( c2m a, c2m b ) { c2m c; c.x = c2MulmvT( a, b.x ); c.y = c2MulmvT( a, b.y ); return c; }

// transform ops
C2_INLINE c2x c2xIdentity( ) { c2x x; x.p = c2V( 0, 0 ); x.r = c2RotIdentity( ); return x; }
C2_INLINE c2v c2Mulxv( c2x a, c2v b ) { return c2Add( c2Mulrv( a.r, b ), a.p ); }
C2_INLINE c2v c2MulxvT( c2x a, c2v b ) { return c2MulrvT( a.r, c2Sub( b, a.p ) ); }
C2_INLINE c2x c2Mulxx( c2x a, c2x b ) { c2x c; c.r = c2Mulrr( a.r, b.r ); c.p = c2Add( c2Mulrv( a.r, b.p ), a.p ); return c; }
C2_INLINE c2x c2MulxxT( c2x a, c2x b ) { c2x c; c.r = c2MulrrT( a.r, b.r ); c.p = c2MulrvT( a.r, c2Sub( b.p, a.p ) ); return c; }

// halfspace ops
C2_INLINE c2v c2Origin( c2h h ) { return c2Mulvs( h.n, h.d ); }
C2_INLINE float c2Dist( c2h h, c2v p ) { return c2Dot( h.n, p ) - h.d; }
C2_INLINE c2v c2Project( c2h h, c2v p ) { return c2Sub( p, c2Mulvs( h.n, c2Dist( h, p ) ) ); }
C2_INLINE c2h c2Mulxh( c2x a, c2h b ) { c2h c; c.n = c2Mulrv( a.r, b.n ); c.d = c2Dot( c2Mulxv( a, c2Origin( b ) ), c.n ); return c; }
C2_INLINE c2h c2MulxhT( c2x a, c2h b ) { c2h c; c.n = c2MulrvT( a.r, b.n ); c.d = c2Dot( c2MulxvT( a, c2Origin( b ) ), c.n ); return c; }
C2_INLINE c2v c2Intersect( c2v a, c2v b, float da, float db ) { return c2Add( a, c2Mulvs( c2Sub( b, a ), (da / (da - db)) ) ); }

#define TINYC2_H
#endif

#ifdef TINYC2_IMPL

int c2Collided( void* A, c2x* ax, C2_TYPE typeA, void* B, c2x* bx, C2_TYPE typeB )
{
	switch ( typeA )
	{
	case C2_CIRCLE:
		switch ( typeB )
		{
		case C2_CIRCLE:  return c2CircletoCircle( *(c2Circle*)A, *(c2Circle*)B );
		case C2_AABB:    return c2CircletoAABB( *(c2Circle*)A, *(c2AABB*)B );
		case C2_CAPSULE: return c2CircletoCapsule( *(c2Circle*)A, *(c2Capsule*)B );
		case C2_POLY:    return c2CircletoPoly( *(c2Circle*)A, (c2Poly*)B, bx );
		default:         return 0;
		}
		break;

	case C2_AABB:
		switch ( typeB )
		{
		case C2_CIRCLE:  return c2CircletoAABB( *(c2Circle*)B, *(c2AABB*)A );
		case C2_AABB:    return c2AABBtoAABB( *(c2AABB*)A, *(c2AABB*)B );
		case C2_CAPSULE: return c2AABBtoCapsule( *(c2AABB*)A, *(c2Capsule*)B );
		case C2_POLY:    return c2AABBtoPoly( *(c2AABB*)A, (c2Poly*)B, bx );
		default:         return 0;
		}
		break;

	case C2_CAPSULE:
		switch ( typeB )
		{
		case C2_CIRCLE:  return c2CircletoCapsule( *(c2Circle*)B, *(c2Capsule*)A );
		case C2_AABB:    return c2AABBtoCapsule( *(c2AABB*)B, *(c2Capsule*)A );
		case C2_CAPSULE: return c2CapsuletoCapsule( *(c2Capsule*)A, *(c2Capsule*)B );
		case C2_POLY:    return c2CapsuletoPoly( *(c2Capsule*)A, (c2Poly*)B, bx );
		default:         return 0;
		}
		break;

	case C2_POLY:
		switch ( typeB )
		{
		case C2_CIRCLE:  return c2CircletoPoly( *(c2Circle*)B, (c2Poly*)A, ax );
		case C2_AABB:    return c2AABBtoPoly( *(c2AABB*)B, (c2Poly*)A, ax );
		case C2_CAPSULE: return c2CapsuletoPoly( *(c2Capsule*)A, (c2Poly*)A, ax );
		case C2_POLY:    return c2PolytoPoly( (c2Poly*)A, ax, (c2Poly*)B, bx );
		default:         return 0;
		}
		break;

	default:
		return 0;
	}
}

void c2Collide( void* A, c2x* ax, C2_TYPE typeA, void* B, c2x* bx, C2_TYPE typeB, c2Manifold* m )
{
	m->count = 0;

	switch ( typeA )
	{
	case C2_CIRCLE:
		switch ( typeB )
		{
		case C2_CIRCLE:  c2CircletoCircleManifold( *(c2Circle*)A, *(c2Circle*)B, m );
		case C2_AABB:    c2CircletoAABBManifold( *(c2Circle*)A, *(c2AABB*)B, m );
		case C2_CAPSULE: c2CircletoCapsuleManifold( *(c2Circle*)A, *(c2Capsule*)B, m );
		case C2_POLY:    c2CircletoPolyManifold( *(c2Circle*)A, (c2Poly*)B, bx, m );
		}
		break;

	case C2_AABB:
		switch ( typeB )
		{
		case C2_CIRCLE:  c2CircletoAABBManifold( *(c2Circle*)B, *(c2AABB*)A, m );
		case C2_AABB:    c2AABBtoAABBManifold( *(c2AABB*)A, *(c2AABB*)B, m );
		case C2_CAPSULE: c2AABBtoCapsuleManifold( *(c2AABB*)A, *(c2Capsule*)B, m );
		case C2_POLY:    c2AABBtoPolyManifold( *(c2AABB*)A, (c2Poly*)B, bx, m );
		}
		break;

	case C2_CAPSULE:
		switch ( typeB )
		{
		case C2_CIRCLE:  c2CircletoCapsuleManifold( *(c2Circle*)B, *(c2Capsule*)A, m );
		case C2_AABB:    c2AABBtoCapsuleManifold( *(c2AABB*)B, *(c2Capsule*)A, m );
		case C2_CAPSULE: c2CapsuletoCapsuleManifold( *(c2Capsule*)A, *(c2Capsule*)B, m );
		case C2_POLY:    c2CapsuletoPolyManifold( *(c2Capsule*)A, (c2Poly*)B, bx, m );
		}
		break;

	case C2_POLY:
		switch ( typeB )
		{
		case C2_CIRCLE:  c2CircletoPolyManifold( *(c2Circle*)B, (c2Poly*)A, ax, m );
		case C2_AABB:    c2AABBtoPolyManifold( *(c2AABB*)B, (c2Poly*)A, ax, m );
		case C2_CAPSULE: c2CapsuletoPolyManifold( *(c2Capsule*)A, (c2Poly*)A, ax, m );
		case C2_POLY:    c2PolytoPolyManifold( (c2Poly*)A, ax, (c2Poly*)B, bx, m );
		}
		break;
	}
}

#define C2_GJK_ITERS 20

typedef struct
{
	float radius;
	int count;
	c2v verts[ C2_MAX_POLYGON_VERTS ];
} c2Proxy;

typedef struct
{
	c2v sA;
	c2v sB;
	c2v p;
	float u;
	int iA;
	int iB;
} c2sv;

typedef struct
{
	c2sv a, b, c, d;
	float div;
	int count;
} c2Simplex;

void c2MakeProxy( void* shape, C2_TYPE type, c2Proxy* p )
{
	switch ( type )
	{
	case C2_CIRCLE:
	{
		c2Circle* c = (c2Circle*)shape;
		p->radius = c->r;
		p->count = 1;
		p->verts[ 0 ] = c->p;
	}	break;

	case C2_AABB:
	{
		c2AABB* bb = (c2AABB*)shape;
		p->radius = 0;
		p->count = 4;
		p->verts[ 0 ] = bb->min;
		p->verts[ 1 ] = c2V( bb->max.x, bb->min.y );
		p->verts[ 2 ] = bb->max;
		p->verts[ 3 ] = c2V( bb->min.x, bb->max.y );
	}	break;

	case C2_CAPSULE:
	{
		c2Capsule* c = (c2Capsule*)shape;
		p->radius = c->r;
		p->count = 2;
		p->verts[ 0 ] = c->a;
		p->verts[ 1 ] = c->b;
	}	break;

	case C2_POLY:
	{
		c2Poly* poly = (c2Poly*)shape;
		p->radius = 0;
		p->count = poly->count;
		for ( int i = 0; i < p->count; ++i ) p->verts[ i ] = poly->verts[ i ];
	}	break;
	}
}

int c2Support( c2v* verts, int count, c2v d )
{
	int imax = 0;
	float dmax = c2Dot( verts[ 0 ], d );

	for ( int i = 1; i < count; ++i )
	{
		float dot = c2Dot( verts[ i ], d );
		if ( dot > dmax )
		{
			imax = i;
			dmax = dot;
		}
	}

	return imax;
}

#define C2_BARY( n, x ) c2Mulvs( s->n.x, (den * s->n.u) )
#define C2_BARY2( x ) c2Add( C2_BARY( a, x ), C2_BARY( b, x ) )
#define C2_BARY3( x ) c2Add( c2Add( C2_BARY( a, x ), C2_BARY( b, x ) ), C2_BARY( c, x ) )

c2v c2L( c2Simplex* s )
{
	float den = 1.0f / s->div;
	switch ( s->count )
	{
	case 1: return s->a.p;
	case 2: return C2_BARY2( p );
	case 3: return C2_BARY3( p );
	default: return c2V( 0, 0 );
	}
}

void c2Witness( c2Simplex* s, c2v* a, c2v* b )
{
	float den = 1.0f / s->div;
	switch ( s->count )
	{
	case 1: *a = s->a.sA; *b = s->a.sB; break;
	case 2: *a = C2_BARY2( sA ); *b = C2_BARY2( sB ); break;
	case 3: *a = C2_BARY3( sA ); *b = C2_BARY3( sB ); break;
	default: *a = c2V( 0, 0 ); *b = c2V( 0, 0 );
	}
}

c2v c2D( c2Simplex* s )
{
	switch ( s->count )
	{
	case 1: return c2Neg( s->a.p );
	case 2:
	{
		c2v ab = c2Sub( s->b.p, s->a.p );
		if ( c2Det2( ab, c2Neg( s->a.p ) ) > 0 ) return c2Skew( ab );
		return c2CW90( ab );
	}
	case 3:
	default: return c2V( 0, 0 );
	}
}

void c22( c2Simplex* s )
{
	c2v a = s->a.p;
	c2v b = s->b.p;
	float u = c2Dot( b, c2Norm( c2Sub( b, a ) ) );
	float v = c2Dot( a, c2Norm( c2Sub( a, b ) ) );

	if ( v <= 0 )
	{
		s->a.u = 1.0f;
		s->div = 1.0f;
		s->count = 1;
	}

	else if ( u <= 0 )
	{
		s->a = s->b;
		s->a.u = 1.0f;
		s->div = 1.0f;
		s->count = 1;
	}

	else
	{
		s->a.u = u;
		s->b.u = v;
		s->div = u + v;
		s->count = 2;
	}
}

void c23( c2Simplex* s )
{
	c2v a = s->a.p;
	c2v b = s->b.p;
	c2v c = s->c.p;

	float uAB = c2Dot( b, c2Norm( c2Sub( b, a ) ) );
	float vAB = c2Dot( a, c2Norm( c2Sub( a, b ) ) );
	float uBC = c2Dot( c, c2Norm( c2Sub( c, b ) ) );
	float vBC = c2Dot( b, c2Norm( c2Sub( b, c ) ) );
	float uCA = c2Dot( a, c2Norm( c2Sub( a, c ) ) );
	float vCA = c2Dot( c, c2Norm( c2Sub( c, a ) ) );
	float area = c2Det2( c2Norm( c2Sub( b, a ) ), c2Norm( c2Sub( c, a ) ) );
	float uABC = c2Det2( b, c ) * area;
	float vABC = c2Det2( c, a ) * area;
	float wABC = c2Det2( a, b ) * area;

	if ( vAB <= 0 && uCA <= 0 )
	{
		s->a.u = 1.0f;
		s->div = 1.0f;
		s->count = 1;
	}

	else if ( uAB <= 0 && vBC <= 0 )
	{
		s->a = s->b;
		s->a.u = 1.0f;
		s->div = 1.0f;
		s->count = 1;
	}

	else if ( uBC <= 0 && vCA <= 0 )
	{
		s->a = s->c;
		s->a.u = 1.0f;
		s->div = 1.0f;
		s->count = 1;
	}

	else if ( uAB > 0 && vAB > 0 && wABC <= 0 )
	{
		s->a.u = uAB;
		s->b.u = vAB;
		s->div = uAB + vAB;
		s->count = 2;
	}

	else if ( uBC > 0 && vBC > 0 && uABC <= 0 )
	{
		s->a = s->b;
		s->b = s->c;
		s->a.u = uBC;
		s->b.u = vBC;
		s->div = uBC + vBC;
		s->count = 2;
	}

	else if ( uCA > 0 && vCA > 0 && vABC <= 0 )
	{
		s->b = s->a;
		s->a = s->c;
		s->a.u = uCA;
		s->b.u = vCA;
		s->div = uCA + vCA;
		s->count = 2;
	}

	else
	{
		s->a.u = uABC;
		s->b.u = vABC;
		s->c.u = wABC;
		s->div = uABC + vABC + wABC;
		s->count = 3;
	}
}

#include <float.h>

float c2GJK( void* A, C2_TYPE typeA, c2x* ax_ptr, void* B, C2_TYPE typeB, c2x* bx_ptr, c2v* outA, c2v* outB, int use_radius )
{
	c2x ax;
	c2x bx;
	if ( typeA != C2_POLY || !ax_ptr ) ax = c2xIdentity( );
	else ax = *ax_ptr;
	if ( typeB != C2_POLY || !bx_ptr ) bx = c2xIdentity( );
	else bx = *bx_ptr;

	c2Proxy pA;
	c2Proxy pB;
	c2MakeProxy( A, typeA, &pA );
	c2MakeProxy( B, typeB, &pB );

	c2Simplex s;
	s.a.iA = 0;
	s.a.iB = 0;
	s.a.sA = c2Mulxv( ax, pA.verts[ 0 ] );
	s.a.sB = c2Mulxv( bx, pB.verts[ 0 ] );
	s.a.p = c2Sub( s.a.sB, s.a.sA );
	s.a.u = 1.0f;
	s.count = 1;

	c2sv* verts = &s.a;
	int saveA[ 3 ], saveB[ 3 ];
	int save_count = 0;
	float d0 = FLT_MAX;
	float d1 = FLT_MAX;
	int iter = 0;
	int hit = 0;
	while ( iter < C2_GJK_ITERS )
	{
		save_count = s.count;
		for ( int i = 0; i < save_count; ++i )
		{
			saveA[ i ] = verts[ i ].iA;
			saveB[ i ] = verts[ i ].iB;
		}
		
		switch ( s.count )
		{
		case 1: break;
		case 2: c22( &s ); break;
		case 3: c23( &s ); break;
		}

		if ( s.count == 3 )
		{
			hit = 1;
			break;
		}

		c2v p = c2L( &s );
		d1 = c2Dot( p, p );

		if ( d1 > d0 ) break;
		d0 = d1;

		c2v d = c2D( &s );
		if ( c2Dot( d, d ) < FLT_EPSILON * FLT_EPSILON ) break;

		int iA = c2Support( pA.verts, pA.count, c2MulrvT( ax.r, c2Neg( d ) ) );
		c2v sA = c2Mulxv( ax, pA.verts[ iA ] );
		int iB = c2Support( pB.verts, pB.count, c2MulrvT( bx.r, d ) );
		c2v sB = c2Mulxv( bx, pB.verts[ iB ] );

		++iter;

		int dup = 0;
		for ( int i = 0; i < save_count; ++i )
		{
			if ( iA == saveA[ i ] && iB == saveB[ i ] )
			{
				dup = true;
				break;
			}
		}
		if ( dup ) break;

		c2sv* v = verts + s.count;
		v->iA = iA;
		v->sA = sA;
		v->iB = iB;
		v->sB = sB;
		v->p = c2Sub( v->sB, v->sA );
		++s.count;
	}

	c2v a, b;
	c2Witness( &s, &a, &b );
	float dist = c2Len( c2Sub( a, b ) );

	if ( hit )
	{
		a = b;
		dist = 0;
	}

	else if ( use_radius )
	{
		float rA = pA.radius;
		float rB = pB.radius;

		if ( dist > rA + rB && dist > FLT_EPSILON )
		{
			dist -= rA + rB;
			c2v n = c2Norm( c2Sub( b, a ) );
			a = c2Add( a, c2Mulvs( n, rA ) );
			b = c2Sub( b, c2Mulvs( n, rB ) );
		}

		else
		{
			c2v p = c2Mulvs( c2Add( a, b ), 0.5f );
			a = p;
			b = p;
			dist = 0;
		}
	}

	if ( outA ) *outA = a;
	if ( outB ) *outB = b;
	return dist;
}

int c2Hull( c2v* verts, int count )
{
	if ( count <= 2 ) return 0;
	count = c2Min( C2_MAX_POLYGON_VERTS, count );

	int right = 0;
	float xmax = verts[ 0 ].x;
	for ( int i = 1; i < count; ++i )
	{
		float x = verts[ i ].x;
		if ( x > xmax )
		{
			xmax = x;
			right = i;
		}

		else if ( x == xmax )
		if ( verts[ i ].y < verts[ right ].y ) right = i;
	}

	int hull[ C2_MAX_POLYGON_VERTS ];
	int out_count = 0;
	int index = right;

	while ( 1 )
	{
		hull[ out_count ] = index;
		int next = 0;

		for ( int i = 1; i < count; ++i )
		{
			if ( next == index )
			{
				next = i;
				continue;
			}

			c2v e1 = c2Sub( verts[ next ], verts[ hull[ out_count ] ] );
			c2v e2 = c2Sub( verts[ i ], verts[ hull[ out_count ] ] );
			float c = c2Det2( e1, e2 );
			if( c < 0 ) next = i;
			if ( c == 0 && c2Dot( e2, e2 ) > c2Dot( e1, e1 ) ) next = i;
		}

		++out_count;
		index = next;
		if ( next == right ) break;
	}

	c2v hull_verts[ C2_MAX_POLYGON_VERTS ];
	for ( int i = 0; i < out_count; ++i ) hull_verts[ i ] = verts[ hull[ i ] ];
	memcpy( verts, hull_verts, sizeof( c2v ) * out_count );
	return out_count;
}

int c2CircletoCircle( c2Circle A, c2Circle B )
{
	c2v c = c2Sub( B.p, A.p );
	float d2 = c2Dot( c, c );
	float r2 = A.r + B.r;
	r2 = r2 * r2;
	if ( d2 < r2 ) return 1;
	return 0;
}

int c2CircletoAABB( c2Circle A, c2AABB B )
{
	c2v L = c2Clampv( A.p, B.min, B.max );
	c2v ab = c2Sub( A.p, L );
	float d2 = c2Dot( ab, ab );
	float r2 = A.r * A.r;
	if ( d2 < r2 ) return 1;
	return 0;
}

int c2AABBtoAABB( c2AABB A, c2AABB B )
{
	c2v d1 = c2Sub( B.min, A.max );
	c2v d2 = c2Sub( A.min, B.max );
	if ( d1.x > 0 || d1.y > 0 ) return 0;
	if ( d2.x > 0 || d2.y > 0 ) return 0;
	return 1;
}

int c2CircletoCapsule( c2Circle A, c2Capsule B )
{
	c2v n = c2Sub( B.b, B.a );
	c2v ap = c2Sub( A.p, B.a );
	float da = c2Dot( ap, n );
	float d2;

	if ( da < 0 ) d2 = c2Dot( ap, ap );
	else
	{
		float db = c2Dot( c2Sub( A.p, B.b ), n );
		if ( db < 0 )
		{
			c2v e = c2Sub( ap, c2Mulvs( n, (da / c2Dot( n, n )) ) );
			d2 = c2Dot( e, e );
		}
		else
		{
			c2v bp = c2Sub( A.p, B.b );
			d2 = c2Dot( bp, bp );
		}
	}

	float r = A.r + B.r;
	if ( d2 < r * r ) return 1;
	return 0;
}

int c2AABBtoCapsule( c2AABB A, c2Capsule B )
{
	if ( c2GJK( &A, C2_AABB, 0, &B, C2_CAPSULE, 0, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2CapsuletoCapsule( c2Capsule A, c2Capsule B )
{
	if ( c2GJK( &A, C2_CAPSULE, 0, &B, C2_CAPSULE, 0, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2CircletoPoly( c2Circle A, c2Poly* B, c2x* bx )
{
	if ( c2GJK( &A, C2_CIRCLE, 0, B, C2_POLY, bx, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2AABBtoPoly( c2AABB A, c2Poly* B, c2x* bx )
{
	if ( c2GJK( &A, C2_AABB, 0, B, C2_POLY, bx, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2CapsuletoPoly( c2Capsule A, c2Poly* B, c2x* bx )
{
	if ( c2GJK( &A, C2_CAPSULE, 0, B, C2_POLY, bx, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2PolytoPoly( c2Poly* A, c2x* ax, c2Poly* B, c2x* bx )
{
	if ( c2GJK( A, C2_POLY, ax, B, C2_POLY, bx, 0, 0, 1 ) ) return 0;
	return 1;
}

int c2RaytoCircle( c2Ray A, c2Circle B, c2Raycast* out )
{
	return 0;
}

int c2RaytoAABB( c2Ray A, c2AABB B, c2Raycast* out )
{
	return 0;
}

int c2RaytoCapsule( c2Ray A, c2Capsule B, c2Raycast* out )
{
	return 0;
}

int c2RaytoPoly( c2Ray A, c2Poly* B, c2x* bx, c2Raycast* out )
{
	return 0;
}

void c2CircletoCircleManifold( c2Circle A, c2Circle B, c2Manifold* m )
{
}

void c2CircletoAABBManifold( c2Circle A, c2AABB B, c2Manifold* m )
{
}

void c2CircletoCapsuleManifold( c2Circle A, c2Capsule B, c2Manifold* m )
{
}

void c2AABBtoAABBManifold( c2AABB A, c2AABB B, c2Manifold* m )
{
}

void c2AABBtoCapsuleManifold( c2AABB A, c2Capsule B, c2Manifold* m )
{
}

void c2CapsuletoCapsuleManifold( c2Capsule A, c2Capsule B, c2Manifold* m )
{
}

void c2CircletoPolyManifold( c2Circle A, c2Poly* B, c2x* bx, c2Manifold* m )
{
}

void c2AABBtoPolyManifold( c2AABB A, c2Poly* B, c2x* bx, c2Manifold* m )
{
}

void c2CapsuletoPolyManifold( c2Capsule A, c2Poly* B, c2x* bx, c2Manifold* m )
{
}

void c2PolytoPolyManifold( c2Poly* A, c2x* ax, c2Poly* B, c2x* bx, c2Manifold* m )
{
}

#endif // TINYC2_IMPL
