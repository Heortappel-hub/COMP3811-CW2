#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
// SOLUTION_TAGS: gl-(ex-[^12]|cw-2|resit)

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator [] allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m[1,2];
 *    m[0,3] = 3.f;
 *
 * (Multi-dimensionsal subscripts in operator[] is a C++23 feature!)
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator[] (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator[] (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f ret;
	for( std::size_t i =0; i <4; ++i )
	{
		for( std::size_t j =0; j <4; ++j )
		{
			float sum =0.f;
			for( std::size_t k =0; k <4; ++k )
				sum += aLeft.v[i*4 + k] * aRight.v[k*4 + j];
			ret.v[i*4 + j] = sum;
		}
	}
	return ret;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f ret{};
	ret.x = aLeft.v[0*4 +0] * aRight.x + aLeft.v[0*4 +1] * aRight.y + aLeft.v[0*4 +2] * aRight.z + aLeft.v[0*4 +3] * aRight.w;
	ret.y = aLeft.v[1*4 +0] * aRight.x + aLeft.v[1*4 +1] * aRight.y + aLeft.v[1*4 +2] * aRight.z + aLeft.v[1*4 +3] * aRight.w;
	ret.z = aLeft.v[2*4 +0] * aRight.x + aLeft.v[2*4 +1] * aRight.y + aLeft.v[2*4 +2] * aRight.z + aLeft.v[2*4 +3] * aRight.w;
	ret.w = aLeft.v[3*4 +0] * aRight.x + aLeft.v[3*4 +1] * aRight.y + aLeft.v[3*4 +2] * aRight.z + aLeft.v[3*4 +3] * aRight.w;
	return ret;
}

// Functions:

Mat44f invert( Mat44f const& aM ) noexcept;

inline
Mat44f transpose( Mat44f const& aM ) noexcept
{
	Mat44f ret;
	for( std::size_t i = 0; i < 4; ++i )
	{
		for( std::size_t j = 0; j < 4; ++j )
			ret[j,i] = aM[i,j];
	}
	return ret;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f m = kIdentity44f;
	float c = std::cos(aAngle);
	float s = std::sin(aAngle);
	m.v[1*4 +1] = c; m.v[1*4 +2] = -s;
	m.v[2*4 +1] = s; m.v[2*4 +2] = c;
	return m;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f m = kIdentity44f;
	float c = std::cos(aAngle);
	float s = std::sin(aAngle);
	m.v[0*4 +0] = c; m.v[0*4 +2] = s;
	m.v[2*4 +0] = -s; m.v[2*4 +2] = c;
	return m;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f m = kIdentity44f;
	float c = std::cos(aAngle);
	float s = std::sin(aAngle);
	m.v[0*4 +0] = c; m.v[0*4 +1] = -s;
	m.v[1*4 +0] = s; m.v[1*4 +1] = c;
	return m;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f m = kIdentity44f;
	m.v[0*4 +3] = aTranslation.x;
	m.v[1*4 +3] = aTranslation.y;
	m.v[2*4 +3] = aTranslation.z;
	return m;
}
inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	Mat44f m{};
	m.v[0] = aSX; m.v[1] =0.f; m.v[2] =0.f; m.v[3] =0.f;
	m.v[4] =0.f; m.v[5] = aSY; m.v[6] =0.f; m.v[7] =0.f;
	m.v[8] =0.f; m.v[9] =0.f; m.v[10]= aSZ; m.v[11]=0.f;
	m.v[12]=0.f; m.v[13]=0.f; m.v[14]=0.f; m.v[15]=1.f;
	return m;
}

inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float f =1.f / std::tan(aFovInRadians *0.5f);
	Mat44f m{};
	m.v[0] = f / aAspect; m.v[1] =0.f; m.v[2] =0.f; m.v[3] =0.f;
	m.v[4] =0.f; m.v[5] = f; m.v[6] =0.f; m.v[7] =0.f;
	m.v[8] =0.f; m.v[9] =0.f; m.v[10] = (aFar + aNear) / (aNear - aFar); m.v[11] = (2.f * aFar * aNear) / (aNear - aFar);
	m.v[12] =0.f; m.v[13] =0.f; m.v[14] = -1.f; m.v[15] =0.f;
	return m;
}

#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
