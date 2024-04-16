#pragma once
#include "IRenderer.h"
#include "base/directX/utility/resource/rendering/shadow/SM_Direction.h"
#include "base/directX/utility/resource/rendering/shadow/SM_Point.h"
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
			const D3D12_GPU_VIRTUAL_ADDRESS& view;	// カメラデータのView
			T* shadow;	// シャドウマップのリソース
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
		void Init(GPUDevice* device, SRV* srv, RootSignature* root, DXC* dxc);

		/// <summary>
		/// 描画命令（平行光源）
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		void DrawCall(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// インデックスデータを追加
		/// </summary>
		void AddIndexData(const IndexInfoStruct& data) { indexBuffer_.Add(data); }
		
		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		void AddTarget(const D3D12_GPU_VIRTUAL_ADDRESS& v, SM_Direction* s) { targetDir_.push_back(Target<SM_Direction>{ v, s }); }
		void AddTarget(const D3D12_GPU_VIRTUAL_ADDRESS& v, SM_Point* s) { targetPoint_.push_back(Target<SM_Point>{ v, s }); }

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
			// インデックスバッファ
		StructuredBuffer<IndexInfoStruct> indexBuffer_;
		// PSO
		PSO pso_;

		// ターゲット（Direction）
		std::vector<Target<SM_Direction>> targetDir_;
		// ターゲット（Point）
		std::vector<Target<SM_Point>> targetPoint_;
	};
}