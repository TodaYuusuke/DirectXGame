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
	indexResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetUsedCount());
	device->CreateShaderResourceView(indexResourceBuffer_->resource_.Get(), &indexDesc, srv_->GetCPUHandle(srv_->GetUsedCount()));
	srv_->AddUsedCount();	// SRV使用数を+1
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
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = stateBefore;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateAfter = stateAfter;

	return barrier;
}