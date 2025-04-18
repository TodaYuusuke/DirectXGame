#pragma once
#include "IRenderingResource.h"

#include "math/vector/Vector2.h"

// 前方宣言
namespace LWP::Resource {
	class Texture;
}

namespace LWP::Base {
	/// <summary>
	/// キューブマップのリソースクラス
	/// </summary>
	class CubeMap
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;

		// SRV上の登録インデックス番号を返す
		int GetSRVIndex() const { return srvInfo.index; }
		// SRV上のViewを返す
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUView() const { return srvInfo.gpuView; }
		// 解像度を返す関数
		Math::Vector2 GetTextureSize() const { return { static_cast<float>(width), static_cast<float>(height) }; }


	public: // ** パブリックなメンバ変数 ** //
		
		// RTVの登録情報（キューブマップなので6枚）
		std::array<RTVInfo, 6> rtvInfos;
		// SRVの登録情報
		SRVInfo srvInfo;


	public: // ** オペレーターオーバーロード ** //

		// Texture型への暗黙の変換演算子をオーバーロード
		//operator Resource::Texture();

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}