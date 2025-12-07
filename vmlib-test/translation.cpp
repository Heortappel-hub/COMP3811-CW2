#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE( "Translation matrix", "[mat44]" )
{
	static constexpr float kEps_ =1e-6f;
	using namespace Catch::Matchers;

	SECTION( "Translate by (1,2,3)" )
	{
		auto const t = make_translation( {1.f,2.f,3.f } );
		Vec4f v {4.f,5.f,6.f,1.f };
		auto r = t * v;
		REQUIRE_THAT( r.x, WithinAbs(5.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(7.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(9.f, kEps_ ) );
		REQUIRE_THAT( r.w, WithinAbs(1.f, kEps_ ) );
	}

	SECTION( "Scaling then Translation composition" )
	{
		auto const s = make_scaling(2.f,3.f,4.f );
		auto const t = make_translation( {1.f,2.f,3.f } );
		auto const m = t * s;
		Vec4f v {1.f,1.f,1.f,1.f };
		auto r = m * v;
		REQUIRE_THAT( r.x, WithinAbs(3.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(5.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(7.f, kEps_ ) );
	}
}

