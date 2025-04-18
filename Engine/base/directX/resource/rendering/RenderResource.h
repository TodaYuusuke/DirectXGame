#pragma once
#include "BackBuffer.h"
#include "math/vector/Vector2.h"
#include "utility/Color.h"

// 前方宣言
namespace LWP::Resource {
	class Texture;
}

namespace LWP::Base {
	/// <summary>
	/// レンダリング結果書き込み用のリソースクラス
	/// </summary>
	class RenderResource
		: public BackBuffer {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 初期化（クリアカラー指定ver）
		void Init(Utility::Color clearColor);

		// UAV対応初期化
		void InitUAV();

		// GPUコライダーのステンシルマスク用初期化
		void InitStencilMask();

		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;

		// SRV上の登録インデックス番号を返す
		int GetSRVIndex() const { return srvInfo.index; }
		// 解像度を返す関数
		Math::Vector2 GetTextureSize() const { return { static_cast<float>(width), static_cast<float>(height) }; }

		// UAV上の登録インデックス番号を返す
		int GetUAVIndex() const { return uavInfo.index; }

	public: // ** パブリックなメンバ変数 ** //
				
		// SRVの登録情報
		SRVInfo srvInfo;
		// UAVの登録情報
		UAVInfo uavInfo;

	public: // ** オペレーターオーバーロード ** //

		// Texture型への暗黙の変換演算子をオーバーロード
		//operator Resource::Texture();

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}