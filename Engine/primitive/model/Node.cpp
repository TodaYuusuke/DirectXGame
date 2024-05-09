#include "Model.h"

#include "component/Resource.h"

using namespace LWP::Primitive;
using namespace LWP::Math;

void Mesh::LoadFile(const std::string& filename) {
	// 頂点とインデックスは先にclearしておく
	vertices.clear();
	indexes.clear();

	// アダプタからリソースマネージャーにアクセスし、読み込んだ結果を受け取る
	*this = Resource::LoadMesh(filename);
}