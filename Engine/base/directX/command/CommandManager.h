#pragma once
#include "Derived/ShadowMapping.h"
#include "../descriptorHeap/HeapManager.h"
#include "RootSignature.h"

#include "renderer/Renderer.h"
#include "postEffect/PostProcessManager.h"

#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")


// 前方宣言
namespace LWP::Object {
	class Camera;
	struct ParticleData;
	class DirectionLight;
	class PointLight;
}

namespace LWP::Base {
	/// <summary>
	/// 描画用コマンドを管理するクラス
	/// </summary>
	class CommandManager {
	public: // メンバ関数

		// コンストラクタ
		CommandManager() = default;
		// デストラクタ
		~CommandManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// ディスクリプタヒープのポインタをセットする関数
		/// </summary>
		void SetDescriptorHeap(HeapManager* manager);
		/// <summary>
		/// ディスクリプタヒープのポインタを返す関数
		/// </summary>
		//RTV* GetRTV() { return rtv_; }
		//SRV* GetSRV() { return srv_; }
		//DSV* GetDSV() { return dsv_; }

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// DrawCall
		/// </summary>
		void DrawCall();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// 描画数リセット
		/// </summary>
		void Reset();


	public: // ** getter ** //
		// CommandQueueを受け取る関数
		ID3D12CommandQueue* GetQueue() const { return commandQueue_.Get(); }
		// メインレンダリング用のコマンドを受け取る関数
		ID3D12GraphicsCommandList* GetMainRenderCommandList() const { return commandList_.Get(); }


	public: // ** 外部からリソースの登録用関数 ** //

		/// <summary>
		/// 描画データ登録関数
		/// </summary>
		void SetDrawData(Primitive::IPrimitive* primitive);
		void SetParticleData(Primitive::IPrimitive* primitive, std::vector<Object::ParticleData>& wtf);
		void SetDrawDataBill2D(Primitive::IPrimitive* primitive);
		void SetDrawDataBill3D(Primitive::IPrimitive* primitive);

		/// <summary>
		/// テクスチャのリソースを作成
		/// </summary>
		ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metadata);
		/// <summary>
		/// レンダリング用のテクスチャのリソースを作成
		/// </summary>
		/// <param name="width">レンダリング解像度（横幅）</param>
		/// <param name="height">レンダリング解像度（縦幅）</param>
		/// <returns></returns>
		ID3D12Resource* CreateTextureResource(const int width, const int height);


		/// <summary>
		/// 描画に使うカメラのビュープロジェクション行列をセットする
		/// </summary>
		void SetMainRendering(Object::Camera* camera);
		/// <summary>
		/// このフレームでレンダリングするサブ画面のコマンドをセットする
		/// </summary>
		void SetSubRendering(Object::Camera* camera);

		/// <summary>
		/// 平行光源のデータを登録する
		/// </summary>
		void SetDirectionLightData(const Object::DirectionLight* light, const Math::Matrix4x4& viewProjections);

		/// <summary>
		/// 点光源のデータを登録する
		/// </summary>
		void SetPointLightData(const Object::PointLight* light, const Math::Matrix4x4* viewProjections);


	private: // メンバ変数
		// デバイスのポインタを保持
		ID3D12Device* device_ = nullptr;
		// ディスクリプタヒープ管理クラスのポインタも保持
		HeapManager* heaps_ = nullptr;

		// コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

		// 用途別コマンドリスト用クラス
		std::unique_ptr<Renderer> renderer_;

		// シャドウマップ計算
		std::vector<std::unique_ptr<ShadowMapping>> shadowCommands_;
		int shadowCount_ = 0;	// 毎フレームの描画回数

		// ポストプロセス管理クラス
		std::unique_ptr<PostProcess::Manager> ppManager_;

		// GPU同期用のフェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;


		// ** 構造体宣言 ** //

		/// <summary>
		/// DirectXCompiler
		/// </summary>
		std::unique_ptr<DXC> dxc_;
		
		// ルートシグネチャ
		std::unique_ptr<RootSignature> rootSignature_;


		// 頂点データ
		std::unique_ptr<IStructured<VertexStruct>> vertexData_;
		// WorldTransformデータ
		std::unique_ptr<IStructured<WTFStruct>> transformData_;

		// 全描画で共通のデータ
		std::unique_ptr<CommonDataResourceBuffer> commonDataResourceBuffer_;
		// マテリアルデータ
		std::unique_ptr<IStructured<MaterialStruct>> materialData_;
		
		// 平行光源
		std::unique_ptr<DirectionLightResourceBuffer> directionLightResourceBuffer_;
		// 点光源
		std::unique_ptr<PointLightResourceBuffer> pointLightResourceBuffer_;

		const UINT kMaxTexture = 128;
		// テクスチャを適応しないとき用のデフォルトのテクスチャ
		Resource::Texture* defaultTexture_;


	private: // ** 非公開メンバ関数 ** //
		CommandManager(const CommandManager&) = delete;
		const CommandManager& operator=(const CommandManager&) = delete;


#pragma region PipelineSet

#pragma region PSO生成関連


		/// <summary>
		/// DXC初期化
		/// </summary>
		void InitializeDXC();
		/// <summary>
		/// RootSignature生成
		/// </summary>
		void CreateRootSignature();

		/// <summary>
		/// ストラクチャーバッファ用のリソースを作成
		/// </summary>
		void CreateStructuredBufferResources();

		/// <summary>
		/// 任意のサイズのResourceを作成
		/// </summary>
		ID3D12Resource* CreateBufferResource(D3D12_RESOURCE_DESC desc, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clearColor);
	};
}