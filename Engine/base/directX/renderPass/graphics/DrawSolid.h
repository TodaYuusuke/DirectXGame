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
	class DrawSolid
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		DrawSolid() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~DrawSolid() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込む
		/// </summary>
		void PushCommand(ID3D12GraphicsCommandList6* list, LWP::Object::Camera** cameras, int size);


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

		// 各モデルタイプごとのルートシグネチャ
		RootSignature root_;
		// 各モデルタイプごとのPSO
		// PSO[0] = Solid
		// PSO[1] = Wireframe
		std::map<Resource::ModelType, PSO[2]> psos_;
	};
}