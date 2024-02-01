#pragma once
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/ResourceStruct.h"
#include "base/directX/resource/structuredBuffer/IStructured.h"
#include "base/directX/command/RootSignature.h"
#include "base/directX/command/PSO.h"
#include "base/directX/descriptorHeap/HeapManager.h"
#include "Config.h"

// 前方宣言
namespace LWP::Object {
	class Camera;
}

namespace LWP::Base {
	// リソースのビューを構造体に
		// 1 ... 共通データ
		// 2 ... 頂点データ
		// 3 ... WorldTransform
		// 4 ... マテリアル
		// 5 ... 平行光源
		// 6 ... 点光源
		// 7 ... テクスチャ
		// 8 ... （平行光源）シャドウマップ
		// 9 ... （点光源）シャドウマップ
	struct ViewStruct {
		D3D12_GPU_VIRTUAL_ADDRESS commonData;
		D3D12_GPU_DESCRIPTOR_HANDLE vertex;
		D3D12_GPU_DESCRIPTOR_HANDLE wtf;
		D3D12_GPU_DESCRIPTOR_HANDLE material;
		D3D12_GPU_DESCRIPTOR_HANDLE directionLight;
		D3D12_GPU_DESCRIPTOR_HANDLE pointLight;
		D3D12_GPU_DESCRIPTOR_HANDLE texture;
		D3D12_GPU_DESCRIPTOR_HANDLE directionShadowMap;
		D3D12_GPU_DESCRIPTOR_HANDLE pointShadowMap;
	};
	// レンダリング先を示す構造体
	struct RenderData {
		LWP::Object::Camera* target;	// レンダリングを行う先のカメラ
		std::unique_ptr<MatrixResourceBuffer> vpResource;	// 計算に使うViewProjection
	};

	/// <summary>
	/// レンダリングコマンド
	/// </summary>
	class MainRenderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		MainRenderer() = default;
		// デストラクタ
		~MainRenderer() = default;

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

		// サブレンダリングのためにRootSignatureを返す関数
		RootSignature* GetRoot() { return root_.get(); }
		// サブレンダリングのためにPSOを返す関数
		PSO* GetPSO() { return pso_.get(); }
		// レンダリングターゲットのカメラクラスを返す関数
		LWP::Object::Camera* GetTarget() { return renderData_->target; }


	private: // ** メンバ変数 ** //

		// レンダリングを行うターゲットのリソース
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