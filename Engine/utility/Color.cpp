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
	G = static_cast<unsigned char>(g);
	B = static_cast<unsigned char>(b);
	A = static_cast<unsigned char>(a);
}

Color::Color(unsigned int c) {
	R = static_cast<unsigned char>(0);
	G = static_cast<unsigned char>(0);
	B = static_cast<unsigned char>(0);
	A = static_cast<unsigned char>(0);
	unsigned char* f[4] = { &A, &B, &G, &R };
	for (int i = 0; i < 4; i++) {
		*f[i] = static_cast<unsigned char>(c % 0x100);
		c /= 0x100;
	}
}

Vector4 Color::GetVector4() {	
	return { static_cast<float>(R) / 255.0f,static_cast<float>(G) / 255.0f,static_cast<float>(B) / 255.0f,static_cast<float>(A) / 255.0f };
}