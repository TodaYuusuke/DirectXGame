#pragma once
#include "IRenderingResource.h"

#include "math/vector/Vector2.h"

// 前方宣言
namespace LWP::Resource {
	class Texture;
}

namespace LWP::Base {
	/// <summary>
	/// 深度キューブマップのリソースクラス
	/// </summary>
	class DepthCubeMap
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init(GPUDevice* device, HeapManager* heaps) override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;


	public: // ** パブリックなメンバ変数 ** //
		
		// DSVの登録情報（キューブマップなので6枚）
		std::array<DSVInfo, 6> dsvInfos;


	public: // ** オペレーターオーバーロード ** //

		// Texture型への暗黙の変換演算子をオーバーロード
		//operator Resource::Texture();

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}