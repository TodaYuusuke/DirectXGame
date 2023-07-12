#include "Color.h"

using namespace LWP::Math;
using namespace LWP::Utility;

Color::Color() {
	R = static_cast<unsigned char>(255);
	G = static_cast<unsigned char>(255);
	B = static_cast<unsigned char>(255);
	A = static_cast<unsigned char>(255);
}

Color::Color(int r, int g, int b, int a) {
	R = static_cast<unsigned char>(r);
	G = static_cast<unsigned char>(b);
	B = static_cast<unsigned char>(g);
	A = static_cast<unsigned char>(a);
}

Vector4 Color::GetVector4() {
	return { static_cast<float>(R),static_cast<float>(G),static_cast<float>(B),static_cast<float>(A) };
}