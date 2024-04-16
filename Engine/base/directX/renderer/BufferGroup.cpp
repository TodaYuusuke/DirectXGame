#include "BufferGroup.h"

using namespace LWP::Base;

void BufferGroup::Init(GPUDevice* device, SRV* srv) {
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
		.Build(device->GetDevice());

	// StructedBuffer
	common_ = std::make_unique<ConstantBuffer<CommonStruct>>();
	common_->Init(device);
	vertex_ = std::make_unique<StructuredBuffer<VertexStruct>>(lwpC::Rendering::kMaxVertex);
	vertex_->Init(device, srv);
	transform_ = std::make_unique<StructuredBuffer<WTFStruct>>(lwpC::Rendering::kMaxMatrix);
	transform_->Init(device, srv);
	material_ = std::make_unique<StructuredBuffer<MaterialStruct>>(lwpC::Rendering::kMaxMaterial);
	material_->Init(device, srv);
	directionLight_ = std::make_unique<StructuredBuffer<DirectionalLightStruct>>(lwpC::Shadow::Direction::kMaxCount);
	directionLight_->Init(device, srv);
	pointLight_ = std::make_unique<StructuredBuffer<PointLightStruct>>(lwpC::Shadow::Point::kMaxCount);
	pointLight_->Init(device, srv);
}

void BufferGroup::SetView(ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootConstantBufferView(2, common_->GetGPUView());
	list->SetGraphicsRootDescriptorTable(3, vertex_->GetGPUView());
	list->SetGraphicsRootDescriptorTable(4, transform_->GetGPUView());
	list->SetGraphicsRootDescriptorTable(5, material_->GetGPUView());
	list->SetGraphicsRootDescriptorTable(6, directionLight_->GetGPUView());
	list->SetGraphicsRootDescriptorTable(7, pointLight_->GetGPUView());
}

void BufferGroup::Reset() {
	vertex_->Reset();
	transform_->Reset();
	material_->Reset();
	directionLight_->Reset();
	pointLight_->Reset();
}
