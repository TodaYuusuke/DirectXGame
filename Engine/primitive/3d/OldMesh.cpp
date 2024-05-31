#include "OldMesh.h"

#include "component/Resource.h"

using namespace LWP::Primitive;
using namespace LWP::Math;

void OldMesh::LoadFile(const std::string& filename) {
	// 頂点とインデックスは先にclearしておく
	vertices.clear();
	indexes.clear();

	// アダプタからリソースマネージャーにアクセスし、読み込んだ結果を受け取る
	OldMeshData* data = Resource::LoadMesh(filename);
	vertices = data->vertices;
	indexes = data->indexes;
	material = data->material;
	material.texture = data->texture;
}