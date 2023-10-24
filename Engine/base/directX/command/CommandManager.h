#pragma once
// 描画用リソースの型を宣言してるヘッダーをinclude
#include "ResourceStruct.h"

#include "PSO.h"
#include "MainCommand.h"
#include "ShadowMapCommand.h"

#include <vector>
#include <memory>
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
		ID3D12GraphicsCommandList* GetMainRenderCommandList() const { return mainCommands_->GetList(); }

	public: // ** 外部からリソースの登録用関数 ** //

		/// <summary>
		/// 汎用描画
		/// </summary>
		void Draw(Primitive::IPrimitive* primitive);

		/// <summary>
		/// マテリアルのリソースを作成
		/// </summary>
		int CreateMaterialResource();
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
		// 用途別コマンドリストたち（現在は１つ）
		std::unique_ptr<MainCommand> mainCommands_;	// 最終レンダリング用
		std::unique_ptr<ShadowMapCommand> shadowMapCommands_;	// シャドウマップ用

		// GPU同期用のフェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;

		// 最大頂点数
		static const int kMaxVertexCount_ = 655350;
		static const int kMaxIndexCount_ = 655350;
		// トランスフォームの許容数
#if _DEBUG //debug時
		static const int kMaxTransformCount_ = 1280;
#else      //release時
		static const int kMaxTransformCount_ = 12800;
#endif
		int usedMatrixCount_ = 0;
		// 最大テクスチャ数（増やす場合はDirectXCommonのコードも修正）
		static const int kMaxTextureCount_ = 128;
		int usedTextureCount_ = 0;


		// ** 構造体宣言 ** //

		struct PipelineSet {
			std::unique_ptr<DXC> dxc_;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;			// リソースとシェーダーのバインディングを定義
			// 数種類のパイプライン
			// 1つめ ... 埋め立てモード（0 -> ワイヤーフレーム、1 -> 埋め立て））
			std::unique_ptr<PSO> pso_[2];	// グラフィックパイプラインの状態を定義
			// シャドウマップ専用のRootSignatureとPSO
			Microsoft::WRL::ComPtr<ID3D12RootSignature> shadowRS_;
			std::unique_ptr<PSO> shadowPSO_;
		};
		std::unique_ptr<PipelineSet> pipelineSet_;


		// 頂点データ
		std::unique_ptr<VertexResourceBuffer> vertexResourceBuffer_;

		// マテリアルデータ
		std::vector<std::unique_ptr<MaterialResourceBuffer>> materialResource_;

		// 行列データ
		std::unique_ptr<MatrixResourceBuffer> matrixResource_[kMaxTransformCount_];
		// カメラのビュープロジェクション用
		// 0 = 2D
		// 1 = 3D
		std::unique_ptr<MatrixResourceBuffer> cameraResource_[2];

		// テクスチャデータ
		std::vector<std::unique_ptr<TextureResourceBuffer>> textureResource_;
		// テクスチャを適応しないとき用のデフォルトのテクスチャ
		Resource::Texture* defaultTexture_;

		// 平行光源
		std::unique_ptr<LightBuffer> lightBuffer_;


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
		/// グラフィックスパイプラインを作成
		/// </summary>
		void CreateGraphicsPipeLineState();


		/// <summary>
		/// 頂点リソースを作成
		/// </summary>
		void CreateVertexResource();
		/// <summary>
		/// 行列のリソースを作成
		/// </summary>
		void CreateMatrixResource();

		/// <summary>
		/// 任意のサイズのResourceを作成
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