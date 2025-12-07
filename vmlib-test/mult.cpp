// You will need to define your own tests. Refer to CW1 or Exercise G.3 for
// examples.

#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE( "Matrix and vector multiplication", "[mat44]" )
{
	static constexpr float kEps_ =1e-6f;
	using namespace Catch::Matchers;

	SECTION( "Identity * Identity = Identity" )
	{
		auto const m = kIdentity44f * kIdentity44f;
		for( std::size_t i =0; i <4; ++i )
			for( std::size_t j =0; j <4; ++j )
				REQUIRE_THAT( (m[i,j]), WithinAbs( kIdentity44f[i,j], kEps_ ) );
	}

	SECTION( "Matrix * Vector" )
	{
		Mat44f m = kIdentity44f;
		m[0,0] =2.f; m[1,1] =3.f; m[2,2] =4.f; m[3,3] =1.f;
		Vec4f v {1.f,2.f,3.f,1.f };
		auto r = m * v;
		REQUIRE_THAT( r.x, WithinAbs(2.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(6.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(12.f, kEps_ ) );
		REQUIRE_THAT( r.w, WithinAbs(1.f, kEps_ ) );
	}

	SECTION( "Composition: Translation then Scaling" )
	{
		auto const t = make_translation( {1.f,2.f,3.f } );
		auto const s = make_scaling(2.f,2.f,2.f );
		auto const m = t * s;
		Vec4f v {1.f,1.f,1.f,1.f };
		auto r = m * v; // scale then translate
		REQUIRE_THAT( r.x, WithinAbs(3.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(4.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(5.f, kEps_ ) );
		REQUIRE_THAT( r.w, WithinAbs(1.f, kEps_ ) );
	}
}

