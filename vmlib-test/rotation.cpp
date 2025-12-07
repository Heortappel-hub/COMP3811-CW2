#include <catch2/catch_amalgamated.hpp>

#include <numbers>

#include "../vmlib/mat44.hpp"

TEST_CASE( "Rotation matrices", "[mat44]" )
{
	static constexpr float kEps_ =1e-6f;
	using namespace Catch::Matchers;

	SECTION( "Rotate90 deg around X" )
	{
		auto const rx = make_rotation_x( std::numbers::pi_v<float> *0.5f );
		Vec4f v {0.f,1.f,0.f,1.f };
		auto r = rx * v;
		REQUIRE_THAT( r.x, WithinAbs(0.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(0.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(1.f, kEps_ ) );
	}

	SECTION( "Rotate90 deg around Y" )
	{
		auto const ry = make_rotation_y( std::numbers::pi_v<float> *0.5f );
		Vec4f v {1.f,0.f,0.f,1.f };
		auto r = ry * v;
		REQUIRE_THAT( r.x, WithinAbs(0.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(0.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(-1.f, kEps_ ) );
	}

	SECTION( "Rotate90 deg around Z" )
	{
		auto const rz = make_rotation_z( std::numbers::pi_v<float> *0.5f );
		Vec4f v {1.f,0.f,0.f,1.f };
		auto r = rz * v;
		REQUIRE_THAT( r.x, WithinAbs(0.f, kEps_ ) );
		REQUIRE_THAT( r.y, WithinAbs(1.f, kEps_ ) );
		REQUIRE_THAT( r.z, WithinAbs(0.f, kEps_ ) );
	}
}

