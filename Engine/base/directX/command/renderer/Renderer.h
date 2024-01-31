#pragma once
#include "MainRenderer.h"
#include "PostProcessRenderer.h"

#include <map>

namespace LWP::Base {
	/// <summary>
	/// レンダリングコマンド
	/// </summary>
	class Renderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		Renderer() = default;
		// デストラクタ
		~Renderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// Viewのリストを渡す
		void SetViewStruct(ViewStruct viewStruct);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list);
		// データをリセット
		void Reset();

		// レンダリングするためのデータをセットする関数
		//void SetSubRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp);
		void SetMainRenderTarget(LWP::Object::Camera* camera);

		// レンダリングに使うデータを追加する関数
		void AddMainRenderData(const IndexInfoStruct& indexInfo);


	private: // ** メンバ変数 ** //

		// メインレンダリング
		std::unique_ptr<MainRenderer> mainRenderer_;

		// サブレンダリング用
		//struct SubRenderData {
		//	std::unique_ptr<RenderData> renderData[lwpC::Rendering::kMaxMultiWindowRendering];	// 計算に使うViewProjection
		//	std::unique_ptr<IStructured<IndexInfoStruct>> indexInfo;
		//};
		//std::unique_ptr<SubRenderData> subRenderData_;

		// ポストプロセス用のルートシグネチャ
		std::unique_ptr<RootSignature> ppRoot_;

		// デバイスポインタ
		ID3D12Device* device_ = nullptr;
		// DirectXコンパイラのポインタ
		DXC* dxc_ = nullptr;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;


	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list, int index);
		void PostDraw(ID3D12GraphicsCommandList* list, int index);
		void PostMainDraw(ID3D12GraphicsCommandList* list, int index);
	};
}