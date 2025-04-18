#include "BufferGroup.h"
#include "component/Information.h"

#include "object/core/light/DirectionLight.h"
#include "object/core/light/PointLight.h"

using namespace LWP::Base;
using namespace LWP::Math;

void BufferGroup::Init() {
	// RootSignature生成
	root_ = std::make_unique<RootSignature>();
	root_->AddTableParameter(0, SV_All)	// インデックスのデータ
		.AddCBVParameter(0, SV_All)	// 描画に使うViewprojection
		.AddCBVParameter(1, SV_All)	// 全画面で共通のデータ
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// トランスフォーム
		.AddTableParameter(1, SV_Pixel)	// マテリアル
		.AddTableParameter(2, SV_Pixel)	// 平行光源
		.AddTableParameter(3, SV_Pixel)	// 点光源
		.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(0, SV_Pixel, 2, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(0, SV_Pixel, 3, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build();

	common_ = std::make_unique<ConstantBuffer<CommonStruct>>();
	common_->Init();
	common_->data_->vp2D = Matrix4x4::CreateIdentity4x4() * Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, LWP::Info::GetWindowWidthF(), LWP::Info::GetWindowHeightF(), 0.0f, 100.0f);
	// StructedBuffer
	vertex_ = std::make_unique<StructuredBuffer<VertexStruct>>(lwpC::Rendering::kMaxVertex);
	vertex_->Init();
	transform_ = std::make_unique<StructuredBuffer<WTFStruct>>(lwpC::Rendering::kMaxMatrix);
	transform_->Init();
	material_ = std::make_unique<StructuredBuffer<MaterialStruct>>(lwpC::Rendering::kMaxMaterial);
	material_->Init();
	directionLight_ = std::make_unique<StructuredBuffer<DirectionalLightStruct>>(lwpC::Shadow::Direction::kMaxCount);
	directionLight_->Init();
	pointLight_ = std::make_unique<StructuredBuffer<PointLightStruct>>(lwpC::Shadow::Point::kMaxCount);
	pointLight_->Init();
}


void BufferGroup::SetCommonView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootConstantBufferView(num, common_->GetGPUView());
}
void BufferGroup::SetVertexView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootDescriptorTable(num, vertex_->GetGPUView());
}
void BufferGroup::SetTransformView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootDescriptorTable(num, transform_->GetGPUView());
}
void BufferGroup::SetMaterialView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootDescriptorTable(num, material_->GetGPUView());
}
void BufferGroup::SetDirLightView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootDescriptorTable(num, directionLight_->GetGPUView());
}
void BufferGroup::SetPointLightView(int num, ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootDescriptorTable(num, pointLight_->GetGPUView());
}

int BufferGroup::AddData(const VertexStruct& data) { return vertex_->Add(data); }
int BufferGroup::AddData(const WTFStruct& data) { return transform_->Add(data); }
int BufferGroup::AddData(const MaterialStruct& data) { return material_->Add(data); }
int BufferGroup::AddData(const DirectionalLightStruct& data) {
	common_->data_->directionLight++;
	return directionLight_->Add(data);
}
int BufferGroup::AddData(const PointLightStruct& data) {
	common_->data_->pointLight++;
	return pointLight_->Add(data);
}

void BufferGroup::Reset() {
	vertex_->Reset();
	transform_->Reset();
	material_->Reset();
	directionLight_->Reset();
	pointLight_->Reset();

	common_->data_->directionLight = 0;
	common_->data_->pointLight = 0;
}
