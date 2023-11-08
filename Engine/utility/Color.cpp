#include "Color.h"
#include "../base/ImGuiManager.h"

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

Color::Color(Vector4 v) {
	R = static_cast<unsigned char>(v.x * 255.0f);
	G = static_cast<unsigned char>(v.y * 255.0f);
	B = static_cast<unsigned char>(v.z * 255.0f);
	A = static_cast<unsigned char>(v.w * 255.0f);
}

Vector4 Color::GetVector4() const {
	return { static_cast<float>(R) / 255.0f,static_cast<float>(G) / 255.0f,static_cast<float>(B) / 255.0f,static_cast<float>(A) / 255.0f };
}

void Color::DebugGUI(const std::string& label) {
	label;
	//LWP::Base::ImGuiManager::ColorEdit4(label.c_str(), *this);
}

Color Color::KelvinToRGB(int kelvin) {
	float temp = kelvin / 100.0f;
	float red, green, blue;

	if (temp <= 66) {
		red = 255;
		green = temp;
		green = 99.4708025861f * log(green) - 161.1195681661f;

		if (temp <= 19)
			blue = 0;
		else {
			blue = temp - 10;
			blue = 138.5177312231f * log(blue) - 305.0447927307f;
		}
	}
	else {
		red = temp - 60;
		red = 329.698727446f * pow(red, -0.1332047592f);

		green = temp - 60;
		green = 288.1221695283f * pow(green, -0.0755148492f);

		blue = 255;
	}

	return Color(static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue), 255);
}