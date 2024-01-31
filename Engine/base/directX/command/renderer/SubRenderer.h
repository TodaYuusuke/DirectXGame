#pragma once
#include "MainRenderer.h"

namespace LWP::Base {
	/// <summary>
	/// サブ画面のレンダリングコマンド
	/// </summary>
	class SubRenderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		SubRenderer() = default;
		// デストラクタ
		~SubRenderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// Viewのリストを渡す
		void SetViewStruct(ViewStruct viewStruct);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list);
		// データをリセット
		void Reset();

		// レンダリングするためのターゲットをセットする関数
		void SetRenderTarget(LWP::Object::Camera* camera);
		// レンダリングするためのデータをセットする関数
		void AddRenderData(const IndexInfoStruct& indexInfo);


	private: // ** メンバ変数 ** //

		// レンダリング先のデータ
		std::unique_ptr<RenderData> renderData_;
		// レンダリングに使うデータ
		std::unique_ptr<IStructured<IndexInfoStruct>> indexInfo_;

		// ルートシグネチャ
		std::unique_ptr<RootSignature> root_;
		// PSO
		std::unique_ptr<PSO> pso_;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;
		// レンダリングにつかうリソースのViewをまとめた構造体
		ViewStruct viewStruct_;


	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list);
		void PostDraw(ID3D12GraphicsCommandList* list);

		void PreLastDraw(ID3D12GraphicsCommandList* list);
		void PostLastDraw(ID3D12GraphicsCommandList* list);
	};
}