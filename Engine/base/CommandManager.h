#pragma once
#include "DirectXCommon.h"

#pragma region 描画用リソースの型を宣言してるクラス

#include "../primitive/IPrimitive.h"	// 頂点
//#include "../resources/material/Material.h"		// マテリアル
//#include "../object/WorldTransform.h"	// 定数
//#include "../resources/texture/Texture.h"	// テクスチャ

#pragma endregion

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>

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
		void Initialize(Base::DirectXCommon* directXCommon);

		/// <summary>
		/// 描画数リセット
		/// </summary>
		void Reset();
		/// <summary>
		/// 描画に使うカメラのポインタをセットする
		/// </summary>
		void SetCameraViewProjection(const Object::Camera* camera);

		/// <summary>
		/// マテリアルのリソースを作成
		/// </summary>
		int CreateMaterialResource();
		/// <summary>
		/// テクスチャのリソースを作成
		/// </summary>
		int CreateTextureResource(const DirectX::ScratchImage& image);


		/// <summary>
		/// 汎用描画
		/// </summary>
		void Draw(Primitive::IPrimitive* primitive);

		/// <summary>
		/// ImGui用
		/// </summary>
		void ImGui();

	private: // メンバ変数
		// DirectXCommon管理
		Base::DirectXCommon* directXCommon_ = nullptr;


		// 最大頂点数
		static const int kMaxVertexCount_ = 655350;
		static const int kMaxIndexCount_ = 655350;
		// トランスフォームの許容数
		static const int kMaxTransformCount_ = 256;
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
		struct VertexStruct {
			Math::Vector4 position_;	// 座標
			Math::Vector2 texCoord_;	// UV座標
			Math::Vector3 normal_;		// 法線
			Math::Vector4 color_;	// 色

			// IPrimitiveのVertexを代入する演算子をオーバーロード
			VertexStruct& operator=(const Primitive::Vertex& value) {
				position_ = { value.position.x,value.position.y,value.position.z,1.0f };
				texCoord_ = value.texCoord;
				normal_ = value.normal;
				color_ = value.color.GetVector4();
				return *this;
			}
		};
		struct VertexResourceBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;	// GPU上の頂点データの格納場所
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};			// BufferLocationは頂点データ格納場所のアドレス
			VertexStruct* vertexData_ = nullptr;	// 頂点リソース
			int usedVertexCount_ = 0;	// 使用済みのインデックスをカウント
			Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;	// 頂点データのインデックス格納場所
			D3D12_INDEX_BUFFER_VIEW indexBufferView_{};			// BufferLocationはインデックス格納場所のアドレス
			uint32_t* indexData_ = nullptr;	// インデックスリソース
			int usedIndexCount_ = 0;	// 使用済みのインデックスをカウント
		};
		std::unique_ptr<VertexResourceBuffer> vertexResourceBuffer_;

		// マテリアルデータ
		struct MaterialStruct {
			Math::Matrix4x4 uvMatrix;
			int32_t enableLighting;

			// Materialクラスのデータを代入する演算子をオーバーロード
			MaterialStruct& operator=(const Resource::Material& value) {
				uvMatrix = value.uvTransform.GetWorldMatrix();
				enableLighting = static_cast<int32_t>(value.enableLighting);
				return *this;
			}
		};
		struct MaterialResourceBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
			D3D12_GPU_VIRTUAL_ADDRESS view_;
			MaterialStruct* data_;
		};
		std::vector<std::unique_ptr<MaterialResourceBuffer>> materialResource_;

		// 行列データ
		struct MatrixResourceBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
			D3D12_GPU_VIRTUAL_ADDRESS view_;
			Math::Matrix4x4* data_;

			// WorldTransformの行列を代入する演算子をオーバーロード
			MatrixResourceBuffer& operator=(const Object::WorldTransform& value) {
				*data_ = value.GetWorldMatrix();
				return *this;
			}
		};
		std::unique_ptr<MatrixResourceBuffer> matrixResource_[kMaxTransformCount_];
		// カメラのビュープロジェクション用
		// 0 = 2D
		// 1 = 3D
		std::unique_ptr<MatrixResourceBuffer> cameraResource_[2];

		// テクスチャデータ
		struct TextureResourceBuffer {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
			D3D12_GPU_DESCRIPTOR_HANDLE view_;
		};
		std::vector<std::unique_ptr<TextureResourceBuffer>> textureResource_;
		// テクスチャを適応しないとき用のデフォルトのテクスチャ
		Resource::Texture* defaultTexture_;

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
	};
}