#include "Material.h"
#include "../../Engine/primitive/PrimitiveManager.h"

using namespace LWP::Resource;

Material::Material(Primitive::Manager* primitiveManager) {
	data_ = new MaterialStruct();
	resource_ = primitiveManager->CreateBufferResource(sizeof(MaterialStruct));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}