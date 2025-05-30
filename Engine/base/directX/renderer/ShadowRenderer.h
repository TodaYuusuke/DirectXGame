#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/shadow/SM_Direction.h"
#include "base/directX/resource/rendering/shadow/SM_Point.h"
#include "base/directX/utility/DXC.h"

#include <vector>

// 前方宣言
namespace LWP::Primitive {
	class IPrimitive;
	class Billboard2D;
	class Billboard3D;
}

template<class T>
concept ShadowT = std::is_base_of<LWP::Base::IShadowMap, T>::value;

namespace LWP::Base {
	/// <summary>
	/// 影のレンダラー
	/// </summary>
	class ShadowRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		template<ShadowT T>
		struct Target {
			D3D12_GPU_VIRTUAL_ADDRESS view;	// カメラデータのView
			T* shadow;	// シャドウマップのリソース
		};
		// 点光源のときだけ特殊なので別ver
		struct TargetPoint {
			std::array<D3D12_GPU_VIRTUAL_ADDRESS, 6> views;	// カメラデータのView
			SM_Point* shadow;	// シャドウマップのリソース
		};

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ShadowRenderer();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~ShadowRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(std::function<void()> func);

		/// <summary>
		/// 描画命令（平行光源）
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		void DrawCall(ID3D12GraphicsCommandList6* list);

		/// <summary>
		/// インデックスデータを追加
		/// </summary>
		void AddIndexData(const IndexInfoStruct& data) { indexBuffer_.Add(data); }
		
		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		void AddTarget(const Target<SM_Direction>& target) { targetDir_.push_back(target); }
		void AddTarget(const TargetPoint& target) { targetPoint_.push_back(target); }

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// インデックスバッファ
		StructuredBuffer<IndexInfoStruct> indexBuffer_;

		// 書き込むデータの種類
		struct RenderData {
			// RootSignature
			RootSignature root;
			// PSO
			PSO pso;
		};

		// 通常
		RenderData normal_;
		// 固体モデル
		RenderData rigid_;
		// スキニングモデル
		RenderData skinning_;


		// ターゲット（Direction）
		std::list<Target<SM_Direction>> targetDir_;
		// ターゲット（Point）
		std::list<TargetPoint> targetPoint_;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 全てのモデルをターゲットに描画する関数
		/// </summary>
		void DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS view);
	};
}