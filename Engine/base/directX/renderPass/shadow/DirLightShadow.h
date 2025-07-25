#pragma once
#include "IShadowPass.h"

#include <map>

namespace LWP {
	namespace Resource {
		enum class ModelType;
	}
}

namespace LWP::Base {
	/// <summary>
	/// MeshShaderで平行光源の影を描画
	/// </summary>
	class DirLightShadow final
		: public IShadowPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		DirLightShadow() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DirLightShadow() override = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込む
		/// </summary>
		void PushCommand(ID3D12GraphicsCommandList6* list);


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 実際の描画命令を積み込む
		/// </summary>
		void SetDispatchMesh(ID3D12GraphicsCommandList6* list);


	private: // ** メンバ変数 ** //

		RootSignature root_;	// ルートシグネチャは共通で使う
		// 各モデルタイプごとのPSO
		std::map<Resource::ModelType, PSO> psos_;
	};
}