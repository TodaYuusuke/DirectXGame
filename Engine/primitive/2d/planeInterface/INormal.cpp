#include "INormal.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	INormal::INormal() {
		name += "Normal";
		Init();
	}
	
	void INormal::FitToTexture() { IPlane::FitToTexture(material.texture.t.GetSize()); }
}