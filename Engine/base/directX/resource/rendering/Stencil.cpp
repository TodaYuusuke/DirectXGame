#include "Stencil.h"

using namespace LWP::Base;

void Stencil::Init() {
	// ※ DXGI_FORMAT_D24_UNORM_S8_UINTがUAVに対応していないため使用不可
	HRESULT hr = S_FALSE;

	// 1. Resourceの設定
	desc.Width = UINT(width);	// Textureの幅
	desc.Height = UINT(height);	// Textureの高さ
	desc.MipLevels = 1; // mipmapの数
	desc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	desc.Format = DXGI_FORMAT_R8_UINT; // Stencilとして利用可能なフォーマット
	desc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; // DepthStencilとして使う通知

	// 3. 利用するHeapの設定。
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 4. バリアを設定（深度値を書き込む状態にしておく）
	currentBarrierState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	// 5. Resourceを生成する
	hr = GPUDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		currentBarrierState,	// バリアの設定
		nullptr,				// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// UAV上に登録
	uavInfo = SRV::GetInstance()->CreateStencilUAV(resource_.Get());
}

void Stencil::Clear(ID3D12GraphicsCommandList* list) {
	// 指定した深度で画面全体をクリアする
	// 何もしない
	list;
}