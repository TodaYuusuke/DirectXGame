#include "RTV.h"

using namespace LWP::Base;
using namespace Microsoft::WRL;

void RTV::Initialize(ID3D12Device* device) {
	device_ = device;
	size_ = 2;
	// 1つ分のサイズを計算
	kDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, size_, false);

	// レンダーターゲットビューの設定
	// シェーダーの計算結果をSRGBに変換して書き込む
	desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
}

void RTV::ClearRenderTarget(UINT backBufferIndex, ID3D12GraphicsCommandList* commandList) {
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得し、指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色。RGBAの順
	commandList->ClearRenderTargetView(GetCPUHandle(backBufferIndex), clearColor, 0, nullptr);
}

//D3D12_CPU_DESCRIPTOR_HANDLE RTV::GetHandle(int index) const {
//	return GetDescriptorHandleIncrementSize(
//		heap_->GetCPUDescriptorHandleForHeapStart(), index,
//		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
//}

