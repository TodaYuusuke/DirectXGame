#pragma once
#include "../base/DirectXCommon.h"
#include "../object/core/Camera.h"
#include "../math/Math.h"

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>

namespace LWP::Object {
	class WorldTransform;
}
namespace LWP::Resource {
	class Texture;
	class Material;
	struct MaterialStruct;
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
		void SetViewProjection2D(Math::Matrix4x4 viewProjection) { *cBuffer_->vpData2D_ = viewProjection; }
		void SetViewProjection3D(Math::Matrix4x4 viewProjection) { *cBuffer_->vpData3D_ = viewProjection; }

		/// <summary>
		/// VertexResourceを作成
		/// </summary>
		ID3D12Resource* CreateBufferResource(size_t);


		/// <summary>
		/// 汎用描画用の引数をまとめた構造体
		/// </summary>
		/// <param name="vertex"> ... 頂点データ</param>
		/// <param name="vertexCount"> ... 頂点データ数</param>
		/// <param name="index"> ... 描画する際の頂点の並び</param>
		/// <param name="fillMode"> ... 描画の埋め立てモード</param>
		/// <param name="worldTransform"> ... ワールドトランスフォーム</param>
		/// <param name="material"> ... マテリアル</param>
		/// <param name="texture"> ... テクスチャ</param>
		/// <param name="isUI"> ... UIとして描画するか</param>
		struct DrawArg {
			Vertex* vertex;
			int vertexCount;
			uint32_t* index;
			int indexCount;
			FillMode fillMode;
			Object::WorldTransform* worldTransform;
			Resource::Material* material;
			Resource::Texture* texture;
			bool isUI;
		};
		/// <summary>
		/// 汎用描画
		/// </summary>
		/// <param name="vertex">頂点データ</param>
		/// <param name="vertexCount">頂点データ数</param>
		/// <param name="index">描画する際の頂点の並び</param>
		/// <param name="fillMode">描画の埋め立てモード</param>
		/// <param name="worldTransform">ワールドトランスフォーム</param>
		/// <param name="material">マテリアル</param>
		/// <param name="texture">テクスチャ</param>
		/// <param name="isUI">UIとして描画するか</param>
		void Draw(Vertex* vertex, int vertexCount, uint32_t* index, int indexCount, FillMode fillMode, Object::WorldTransform* worldTransform, Resource::Material* material, Resource::Texture* texture, bool isUI);

		/// <summary>
		/// ImGui用
		/// </summary>
		void ImGui();

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
			Microsoft::WRL::ComPtr<ID3D12Resource> vpResource2D_;	// 2D用の定数バッファ
			Microsoft::WRL::ComPtr<ID3D12Resource> vpResource3D_;	// 3D用の定数バッファ
			Math::Matrix4x4* vpData2D_ = nullptr;	// 2D用の定数リソース
			Math::Matrix4x4* vpData3D_ = nullptr;	// 3D用の定数リソース
		};

		// 平行光源
		struct DirectionalLight {
			Math::Vector4 color_;		// ライトの色
			Math::Vector3 direction_;	// ライトの向き
			float intensity_;	// 輝度
		};
		// 光源のバッファ
		struct LightBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;	// 3D用の定数バッファ
			DirectionalLight* light_ = nullptr;	// 2D用の定数リソース
		};

		struct VectorPosColor {
			Math::Vector4 position_;	// 座標
			Math::Vector2 texCoord_;	// UV座標
			Math::Vector3 normal_;		// 法線
			Math::Vector4 color_;	// 色
		};
		struct PrimitiveVertex {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;	// GPU上の頂点データの格納場所
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};			// BufferLocationは頂点データ格納場所のアドレス
			VectorPosColor* vertexData_ = nullptr;	// 頂点リソース
			int usedVertexCount_ = 0;	// 使用済みのインデックス
			Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;	// 頂点データのインデックス格納場所
			D3D12_INDEX_BUFFER_VIEW indexBufferView_{};			// BufferLocationはインデックス格納場所のアドレス
			uint32_t* indexData_ = nullptr;	// インデックスリソース
			int usedIndexCount_ = 0;	// 使用済みのインデックス
		};

		// 描画関連
		std::unique_ptr<DXC> dxc_;
		std::unique_ptr<PipelineSet> pipelineSet_;
		std::unique_ptr<CBuffer> cBuffer_;
		std::unique_ptr<LightBuffer> lightBuffer_;
		// 形状の頂点データ
		std::unique_ptr<PrimitiveVertex> primitiveVertex_;

		// テクスチャを適応しないとき用のテクスチャ
		Resource::Texture* defaultTexture_;
		

		// 最大頂点数
		static const int kMaxVertexCount_ = 65535;
		static const int kMaxIndexCount_ = 65535;

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
		void CreateVertexBufferView();

		/// <summary>
		/// 三角形の頂点バッファを作成
		/// </summary>
		void CreatePrimitiveVertex();

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}