#include "ICommand.h"
#include "../descriptorHeap/SRV.h"

using namespace LWP::Base;

void ICommand::Initialize(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature, ID3D12Resource* resource) {
	CreatePSO(device, dxc, rootSignature);

	// インデックス情報の構造体
	indexResourceBuffer_ = std::make_unique<IndexResourceBuffer>();
	indexResourceBuffer_->resource_ = resource;
	indexResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&indexResourceBuffer_->data_));
	
	D3D12_SHADER_RESOURCE_VIEW_DESC indexDesc{};
	indexDesc.Format = DXGI_FORMAT_UNKNOWN;
	indexDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	indexDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	indexDesc.Buffer.FirstElement = 0;
	indexDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	indexDesc.Buffer.NumElements = kMaxIndex;
	indexDesc.Buffer.StructureByteStride = sizeof(IndexInfoStruct);
	indexResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device->CreateShaderResourceView(indexResourceBuffer_->resource_.Get(), &indexDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));

	// 派生クラスの初期化を呼び出し
	DerivedInitialize();
}
void ICommand::DerivedInitialize() { /* 基底クラスでは実装無し */ }

void ICommand::Draw(ID3D12RootSignature* rootSignature, ID3D12GraphicsCommandList* list, std::function<void()> ExecuteLambda, ViewStruct viewStruct) {
	for (currentRenderingCount_ = 0; currentRenderingCount_ < multiRenderingCount_; currentRenderingCount_++) {
		// 描画前処理
		PreDraw(list);
		
		// RootSignatureを設定。PSOに設定してるけど別途設定が必要
		list->SetGraphicsRootSignature(rootSignature);
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// PSOを設定
		list->SetPipelineState(pso_->state_.Get());

		// ディスクリプタテーブルを登録
		list->SetGraphicsRootDescriptorTable(0, indexResourceBuffer_->view_);
		list->SetGraphicsRootConstantBufferView(1, viewStruct.structCount);
		list->SetGraphicsRootConstantBufferView(2, viewStruct.directionLight);
		list->SetGraphicsRootDescriptorTable(3, viewStruct.pointLight);
		list->SetGraphicsRootDescriptorTable(4, viewStruct.vertex);
		list->SetGraphicsRootDescriptorTable(5, viewStruct.cameraVP);
		list->SetGraphicsRootDescriptorTable(6, viewStruct.wtf);
		list->SetGraphicsRootDescriptorTable(7, viewStruct.lightVP);
		list->SetGraphicsRootDescriptorTable(8, viewStruct.material);
		list->SetGraphicsRootDescriptorTable(9, viewStruct.texture);
		list->SetGraphicsRootDescriptorTable(10, viewStruct.directionShadowMap);
		list->SetGraphicsRootDescriptorTable(11, viewStruct.pointShadowMap);

		// 全三角形を１つのDrawCallで描画
		list->DrawInstanced(3, indexResourceBuffer_->usedCount_ / 3, 0, 0);

		PostDraw(list);

		ExecuteLambda();
	}
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
	// 遷移前（現在）のResourceState
	barrier.Transition.StateAfter = stateAfter;

	return barrier;
}