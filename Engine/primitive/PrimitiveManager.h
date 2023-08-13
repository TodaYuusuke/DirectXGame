#pragma once
#include "../base/DirectXCommon.h"
#include "../object/core/Camera.h"
#include "../math/Math.h"

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>

namespace LWP::Resource {
	class Texture;
}

namespace LWP::Primitive {
	enum FillMode : int;

	struct Vertex;

	class Manager {
	public: // メンバ関数

		// コンストラクタ
		Manager() = default;
		// デストラクタ
		~Manager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Base::DirectXCommon* directXCommon);

		/// <summary>
		/// 描画数リセット
		/// </summary>
		void Reset();
		/// <summary>
		/// 描画に使うカメラのポインタをセットする
		/// </summary>
		void SetViewProjection(Math::Matrix4x4 viewProjection) { *cBuffer_->vpData_ = viewProjection; }

		/// <summary>
		/// 汎用描画
		/// </summary>
		void Draw(Vertex* vertex, int vertexCount, FillMode fillMode, Resource::Texture* texture);


	private: // メンバ変数
		// DirectXCommon管理
		Base::DirectXCommon* directXCommon_ = nullptr;

		// 構造体宣言

		//*** DirectXシェーダコンパイラ ***//
		// HLSLコードをバイナリ形式のGPUシェーダーに変換する
		struct DXC {
			Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;				// dxcの汎用オブジェクト
			Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;			// dxcのコンパイラオブジェクト
			Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;	// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
		};
		struct PipelineSet {
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;			// リソースとシェーダーのバインディングを定義
			Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWireFrame_;	// グラフィックパイプラインの状態（WireFrame）を定義
			Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateFill_;	// グラフィックパイプラインの状態（Fill）を定義
		};
		struct CBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> vpResource_;	// 定数バッファ
			Math::Matrix4x4* vpData_;	// 定数リソース
		};

		struct VectorPosColor {
			Math::Vector4 position_;	// 座標
			Math::Vector2 texCoord_;	// UV座標
			Math::Vector4 color_;	// 色
		};
		struct PrimitiveVertex {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;	// GPU上の頂点データの格納場所
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};			// BufferLocationは頂点データ格納場所のアドレス
			VectorPosColor* vertexData_ = nullptr;	// 頂点リソース
		};

		// 描画関連
		std::unique_ptr<DXC> dxc_;
		std::unique_ptr<PipelineSet> pipelineSet_;
		std::unique_ptr<CBuffer> cBuffer_;
		// 形状の頂点データ
		std::unique_ptr<PrimitiveVertex> primitiveVertex_;

		// テクスチャを適応しないとき用のテクスチャ
		Resource::Texture* defaultTexture_;
		

		// 最大頂点数
		static const int kMaxVertexCount = 256;
		// 使用済みのインデックス
		int vertexIndex = 0;

	private: // 非公開のメンバ関数
		Manager(const Manager&) = delete;
		const Manager& operator=(const Manager&) = delete;

		/// <summary>
		/// DXC初期化
		/// </summary>
		void InitialzieDXC();

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
		/// 定数バッファを作成
		/// </summary>
		void CreateConstantBuffer();

		/// <summary>
		/// 頂点バッファビューを作成
		/// </summary>
		void CreateVertexTriangleBufferView();

		/// <summary>
		/// 三角形の頂点バッファを作成
		/// </summary>
		void CreatePrimitiveVertex();


		/// <summary>
		/// VertexResourceを作成
		/// </summary>
		ID3D12Resource* CreateBufferResource(size_t);

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}