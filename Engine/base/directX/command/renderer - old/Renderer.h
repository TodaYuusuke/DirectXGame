#pragma once
#include "MainRenderer.h"
#include "SubRenderer.h"
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
		void SetMainRenderTarget(LWP::Object::Camera* camera);
		// レンダリングするためのデータをセットする関数
		void SetSubRenderTarget(LWP::Object::Camera* camera);

		// レンダリングに使うデータを追加する関数
		void AddRenderData(const IndexInfoStruct& indexInfo, const bool& isWireFrame, const bool& isMain);
		void AddRenderData2DBill(const IndexInfoStruct& indexInfo);
		void AddRenderData3DBill(const IndexInfoStruct& indexInfo);

	private: // ** メンバ変数 ** //

		// メインレンダリング
		std::unique_ptr<MainRenderer> mainRenderer_;
		
		// サブレンダリング
		std::vector<std::unique_ptr<SubRenderer>> subRenderer_;
		// サブレンダリング用IndexInfo
		std::unique_ptr<IStructured<IndexInfoStruct>> subIndexInfoSolid_;
		std::unique_ptr<IStructured<IndexInfoStruct>> subIndexInfoWire_;
		// サブレンダリングの回数
		Utility::Counter subCount_;

		// ポストプロセス用のルートシグネチャ
		std::unique_ptr<RootSignature> ppRoot_;

		// デバイスポインタ
		ID3D12Device* device_ = nullptr;
		// DirectXコンパイラのポインタ
		DXC* dxc_ = nullptr;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;


	private: // ** メンバ変数 ** //
	};
}