#include "ICommand.h"
#include "../descriptorHeap/SRV.h"

#include <Config.h>

using namespace LWP::Base;

void ICommand::Initialize(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	CreatePSO(device, dxc, rootSignature);

	// インデックスデータのResource生成
	indexData_ = std::make_unique<IStructured<IndexInfoStruct>>(device, srv_, lwpC::Rendering::kMaxIndex);
	// viewProjectionのResource生成
	vpResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	vpResourceBuffer_->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(Math::Matrix4x4));
	vpResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&vpResourceBuffer_->data_));
	vpResourceBuffer_->view_ = vpResourceBuffer_->resource_->GetGPUVirtualAddress();

	// 派生クラスの初期化を呼び出し
	DerivedInitialize();
}
void ICommand::DerivedInitialize() { /* 基底クラスでは実装無し */ }

void ICommand::SetDrawData(IndexInfoStruct data) {
	indexData_->AddData(data);
}

void ICommand::Draw(ID3D12RootSignature* rootSignature, ID3D12GraphicsCommandList* list, ViewStruct viewStruct) {
	// 描画前処理
	PreDraw(list);

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(rootSignature);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// PSOを設定
	list->SetPipelineState(pso_->state_.Get());

	// ディスクリプタテーブルを登録
	list->SetGraphicsRootDescriptorTable(0, indexData_->GetView());
	list->SetGraphicsRootConstantBufferView(1, vpResourceBuffer_->view_);
	list->SetGraphicsRootConstantBufferView(2, viewStruct.commonData);
	list->SetGraphicsRootDescriptorTable(3, viewStruct.vertex);
	list->SetGraphicsRootDescriptorTable(4, viewStruct.wtf);
	list->SetGraphicsRootDescriptorTable(5, viewStruct.material);
	list->SetGraphicsRootDescriptorTable(6, viewStruct.directionLight);
	list->SetGraphicsRootDescriptorTable(7, viewStruct.pointLight);
	list->SetGraphicsRootDescriptorTable(8, viewStruct.texture);
	list->SetGraphicsRootDescriptorTable(9, viewStruct.directionShadowMap);
	list->SetGraphicsRootDescriptorTable(10, viewStruct.pointShadowMap);

	// 全三角形を１つのDrawCallで描画
	list->DrawInstanced(3, indexData_->GetCount() / 3, 0, 0);

	PostDraw(list);
}

void ICommand::PreDraw(ID3D12GraphicsCommandList* list) {
	/* 基底クラスでは記述無し */
	list;
}
void ICommand::PostDraw(ID3D12GraphicsCommandList* list) {
	/* 基底クラスでは記述無し */
	list;
}

void ICommand::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	/* 基底クラスでは記述無し */
	device;
	dxc;
	rootSignature;
};


D3D12_RESOURCE_BARRIER ICommand::MakeResourceBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// 全てのサブリソースを選択
	barrier.Transition.Subresource = 0xFFFFFFFF;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = stateBefore;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = stateAfter;

	return barrier;
}