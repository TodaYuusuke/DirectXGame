#include "Vertex.h"

#include "math/matrix/Matrix3x3.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Primitive;

// Vertexに戻すためのオペレータオーバーロード
Vertex& Vertex::operator=(const OutputVertexStruct& value) {
	position = { value.position.x,value.position.y,value.position.z };
	texCoord = value.texCoord;
	normal = value.normal;
	color = value.color;
	materialIndex = value.mIndex;
	return *this;
}

void OutputVertexStruct::ApplyWorldTransform(const Matrix4x4& affine) {
	// X,Y,Z に　アフィン行列を掛ける
	position *= affine;
	worldPosition = Vector3{ position.x,position.y,position.z };
	//texCoord; これはそのまま
	normal = normal * Matrix3x3(affine).Inverse();
	//color; これはそのまま
	//mIndex; これはそのまま
}


