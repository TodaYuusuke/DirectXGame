#include "Albedo.h"

namespace LWP::Base::GBufferTarget {
	void Albedo::Init() {
		HRESULT hr = S_FALSE;

		// 1. Resourceの設定
		desc.Width = UINT(width);
		desc.Height = UINT(height);
		desc.MipLevels = 1;
		desc.DepthOrArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleDesc.Count = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		// 2．クリアカラー（黒）
		clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 0.0f;

		// 3. 利用するHeapの設定
		properties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト

		// 4. バリアを設定
		currentBarrierState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		// 5. Resourceを生成する
		hr = GPUDevice::GetInstance()->GetDevice()->CreateCommittedResource(
			&properties,			// Heapの設定
			D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
			&desc,					// Resourceの設定
			currentBarrierState,	// 初回のResourceState。RTVで書き込むのでちょっとちがう設定
			&clearValue,			// Clear最適地。使わないならnullptr
			IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
		);
		assert(SUCCEEDED(hr));

		// RTV上に登録
		rtvInfo = RTV::GetInstance()->CreateRenderTargetView(resource_.Get());
		// SRV上に登録
		srvInfo = SRV::GetInstance()->CreateRenderResource(resource_.Get());
	}
	void Albedo::Clear(ID3D12GraphicsCommandList* list) {
		ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		list->ClearRenderTargetView(rtvInfo.cpuView, clearValue.Color, 0, nullptr);
	}
	} // namespace LWP::Base::GBufferTarget