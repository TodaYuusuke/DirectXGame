#pragma once
#include "IGraphicsPass.h"

#include <map>

namespace LWP {
	namespace Object {
		class Camera;
	}
	namespace Resource {
		enum class ModelType;
	}
}

namespace LWP::Base {
	/// <summary>
	/// MeshShaderでSolidオブジェクトを描画
	/// </summary>
	class DrawSolid final
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		DrawSolid() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DrawSolid() override = default;

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
		/// 共通で利用するレンダリング用のバッファーを設定する
		/// </summary>
		void SetBuffers(ID3D12GraphicsCommandList6* list);
		/// <summary>
		/// 実際の描画命令を積み込む
		/// </summary>
		void SetDispatchMesh(ID3D12GraphicsCommandList6* list);


	private: // ** メンバ変数 ** //

		RootSignature root_;	// ルートシグネチャは共通で使う

		struct RasterPSO {
			PSO solid;			// ソリッド描画
			PSO wireframe;		// ワイヤーフレーム描画
		};
		// 各モデルタイプごとのPSO
		std::map<Resource::ModelType, RasterPSO> raster_;
	};
}