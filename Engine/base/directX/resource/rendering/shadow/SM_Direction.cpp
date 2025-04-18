#include "SM_Direction.h"

using namespace LWP::Base;
	
void SM_Direction::Init() {
	HRESULT hr = S_FALSE;

	// 解像度
	width = static_cast<int>(1024.0f * lwpC::Shadow::kResolutionScale);
	height = static_cast<int>(1024.0f * lwpC::Shadow::kResolutionScale);

	// 1. Resourceの設定
	desc.Width = width; // Textureの幅
	desc.Height = height; // Textureの高さ
	desc.MipLevels = 1; // mipmapの数	
	desc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	desc.Format = DXGI_FORMAT_D32_FLOAT; // DepthStencilとして利用可能なフォーマット
	desc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 2．クリア値
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	clearValue.DepthStencil.Stencil = 0;

	// 3. 利用するHeapの設定。
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 4. バリアを設定（デフォルトは読み取り専用）
	currentBarrierState = D3D12_RESOURCE_STATE_GENERIC_READ;

	// 5. Resourceを生成する
	hr = GPUDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		currentBarrierState,	// バリアの設定
		&clearValue,				// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// SRV上に登録
	srvInfo = SRV::GetInstance()->CreateShadowMapDir(resource_.Get());
	// DSV上に登録
	dsvInfo = DSV::GetInstance()->CreateShadowMapDir(resource_.Get());
}

void SM_Direction::Clear(ID3D12GraphicsCommandList* list) {
	// 指定した深度で画面全体をクリアする
	list->ClearDepthStencilView(dsvInfo.cpuView, D3D12_CLEAR_FLAG_DEPTH, clearValue.DepthStencil.Depth, 0, 0, nullptr);
}
