#pragma once
// 描画用リソースの型を宣言してるヘッダーをinclude
#include "ResourceStruct.h"

#include "Command.h"

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
		void PreDraw(UINT backBufferIndex, ID3D12Resource* backBuffer);

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(ID3D12Resource* backBuffer, IDXGISwapChain4* swapChain);

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
		ID3D12GraphicsCommandList* GetMainRenderCommandList() const { return commands_->GetList(); }

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
		/// 描画に使うカメラのポインタをセットする
		/// </summary>
		void SetCameraViewProjection(const Object::Camera* camera);


	private: // メンバ変数
		// デバイスのポインタだけ貰う
		ID3D12Device* device_ = nullptr;
		// 各ディスクリプタヒープのポインタも保持
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;

		// コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		// コマンドクラスの配列（現在は１つ）
		std::unique_ptr<Command> commands_;

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

		//*** DirectXシェーダコンパイラ ***//
		// HLSLコードをバイナリ形式のGPUシェーダーに変換する
		struct DXC {
			Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;				// dxcの汎用オブジェクト
			Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;			// dxcのコンパイラオブジェクト
			Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;	// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
		};
		std::unique_ptr<DXC> dxc_;
		struct PipelineSet {
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;			// リソースとシェーダーのバインディングを定義
			// 数種類のパイプライン
			// 1つめ ... 埋め立てモード（0 -> ワイヤーフレーム、1 -> 埋め立て））
			Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_[2];	// グラフィックパイプラインの状態を定義
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

		/// <summary>
		/// DXC初期化
		/// </summary>
		void InitializeDXC();

#pragma region PipelineSet

#pragma region PSO生成関連

		void CreateRootSignature();
		D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
		D3D12_BLEND_DESC CreateBlendState();
		D3D12_RASTERIZER_DESC CreateRasterizerState();
		IDxcBlob* CreateVertexShader();
		IDxcBlob* CreatePixelShader();
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

#pragma endregion

		/// <summary>
		/// グラフィックスパイプラインを作成
		/// </summary>
		void CreateGraphicsPipeLineState();

#pragma endregion

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

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

		/// <summary>
		/// リソースバリアの実態を作る関数
		/// </summary>
		D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);
	};
}