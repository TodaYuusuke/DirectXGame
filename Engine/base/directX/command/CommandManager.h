#pragma once
#include "MainCommand.h"
#include "ShadowMapCommand.h"

#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

// 前方宣言
namespace LWP::Object {
	class Camera;
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
		void SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv);

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

		/// <summary>
		/// ImGui用
		/// </summary>
		void ImGui();


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

		/// <summary>
		/// テクスチャのリソースを作成
		/// </summary>
		int CreateTextureResource(const DirectX::ScratchImage& image);

		/// <summary>
		/// 描画に使うカメラのビュープロジェクション行列をセットする
		/// </summary>
		void SetCameraViewProjection(const Object::Camera* camera);


	private: // メンバ変数
		// デバイスのポインタを保持
		ID3D12Device* device_ = nullptr;
		// 各ディスクリプタヒープのポインタも保持
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;

		// コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

		// 用途別コマンドリスト用クラス（現在は１つ）
		std::vector<ICommand*> cmds_;

		// GPU同期用のフェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;


		// ** 構造体宣言 ** //

		/// <summary>
		/// DirectXCompiler
		/// </summary>
		std::unique_ptr<DXC> dxc_;
		
		/// <summary>
		// ディスクリプタテーブル番号
		// <para>0 ... バッファーのインデックス</para>
		// <para>1 ... 平行光源</para>
		// <para>2 ... 頂点データ</para>
		// <para>3 ... カメラのviewProjection</para>
		// <para>4 ... WorldTransform</para>
		// <para>5 ... マテリアル</para>
		// <para>6 ... シャドウマップ</para>
		// <para>7 ... テクスチャ</para>
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;


		// 平行光源
		std::unique_ptr<LightResourceBuffer> lightResourceBuffer_;
		//const UINT kMaxLight= 1;

		// 頂点データ
		std::unique_ptr<VertexResourceBuffer> vertexResourceBuffer_;
		const UINT kMaxVertex = 655350;
		// カメラのビュープロジェクション用
		// 0 = 2D
		// 1 = 3D
		std::unique_ptr<MatrixResourceBuffer> cameraResourceBuffer_;
		const UINT kMaxCameraVP = 1 * 2;
		// WorldTransformデータ
		std::unique_ptr<MatrixResourceBuffer> matrixResourceBuffer_;
#if _DEBUG //debug時
		const UINT kMaxMatrix = 1280;
#else      //release時
		const UINT kMaxMatrix = 12800;
#endif


		// マテリアルデータ
		std::unique_ptr<MaterialResourceBuffer> materialResourceBuffer_;
		const UINT kMaxMaterial = 12800;
		// テクスチャデータ
		std::unique_ptr<TextureResourceBuffer> textureResourceBuffer_;
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
		/// シャドウマップ用のRootSignature生成
		/// </summary>
		void CreateShadowRS();

		/// <summary>
		/// ストラクチャーバッファ用のリソースを作成
		/// </summary>
		void CreateStructuredBufferResources();

		/// <summary>
		/// 任意のサイズのResourceを作成する関数
		/// </summary>
		ID3D12Resource* CreateBufferResource(size_t size);
		/// <summary>
		/// 任意のサイズのResourceを作成（Textureバージョン）
		/// </summary>
		ID3D12Resource* CreateBufferResource(const DirectX::TexMetadata& metadata);
		/// <summary>
		/// テクスチャをアップロード
		/// </summary>
		void UploadTextureData(const DirectX::ScratchImage& mipImages);

	};
}