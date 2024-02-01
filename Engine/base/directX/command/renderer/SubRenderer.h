#pragma once
#include "MainRenderer.h"

// 前方宣言
namespace LWP::Object {
	class Camera;
}

namespace LWP::Base {
	/// <summary>
	/// サブ画面レンダリングコマンド
	/// </summary>
	class SubRenderer {
	public: // ** メンバ関数 ** //
		// コンストラクタ
		SubRenderer() = default;
		// デストラクタ
		~SubRenderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, RootSignature* rootPtr, PSO* psoPtr, HeapManager* heaps);
		// Viewのリストを渡す
		void SetViewStruct(ViewStruct viewStruct, D3D12_GPU_DESCRIPTOR_HANDLE indexInfoView);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list, int instanceCount);

		// レンダリングするためのターゲットをセットする関数
		void SetRenderTarget(LWP::Object::Camera* camera);
		
		// レンダリングターゲットのカメラクラスを返す関数
		LWP::Object::Camera* GetTarget() { return renderData_->target; }


	private: // ** メンバ変数 ** //

		// レンダリングを行うターゲットのリソース
		std::unique_ptr<RenderData> renderData_;
		
		// 共通のルートシグネチャのポインタ
		RootSignature* rootPtr_;
		// 共通のPSOのポインタ
		PSO* psoPtr_;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;
		// レンダリングにつかうリソースのViewをまとめた構造体
		ViewStruct viewStruct_;
		// IndexInfo用のView
		D3D12_GPU_DESCRIPTOR_HANDLE indexInfoView_;


	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list);
		void PostDraw(ID3D12GraphicsCommandList* list);
	};
}