#pragma once
#include "GPUDevice.h"
#include "DXC.h"

#include <DirectXTex/d3dx12.h>

#include <string>
#include <initializer_list>

namespace LWP::Base {
	/// <summary>
	/// グラフィックスパイプライン
	/// </summary>
	class PSO final {
	public: // ** 設定用の列挙子 ** //
		enum class Type {
			Vertex,
			Compute,
			Mesh
		};
		// フィルモード
		enum class FillMode {
			Solid,
			Wireframe,
			Count
		};
		// ブレンドモード
		enum class BlendMode {
			Normal,
			Add,
			Subtract,
			Multiply,
			Screen
		};
		// 設定
		union Desc {
			D3D12_GRAPHICS_PIPELINE_STATE_DESC vertex{};
			D3D12_COMPUTE_PIPELINE_STATE_DESC compute;
			D3DX12_MESH_SHADER_PIPELINE_STATE_DESC mesh;

			Desc() {};
			~Desc() {};
		};


	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		PSO& Init(ID3D12RootSignature* root, Type type = Type::Vertex);
		PSO& SetRTVFormats(const DXGI_FORMAT& format) { return SetRTVFormats({ format }); }
		PSO& SetRTVFormats(std::initializer_list<DXGI_FORMAT> formats);
		PSO& SetInputLayout();
		PSO& SetBlendState(bool enable, BlendMode mode = BlendMode::Normal);
		PSO& SetRasterizerState(
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID);
		PSO& SetAS(std::string filePath);
		PSO& SetSystemAS(std::string filePath);
		PSO& SetMS(std::string filePath);
		PSO& SetSystemMS(std::string filePath);
		PSO& SetCS(std::string filePath);
		PSO& SetSystemCS(std::string filePath);
		PSO& SetVS(std::string filePath);
		PSO& SetSystemVS(std::string filePath);
		PSO& SetPS(std::string filePath);
		PSO& SetSystemPS(std::string filePath);
		PSO& SetDepthState(bool enable,
			D3D12_DEPTH_WRITE_MASK mask = D3D12_DEPTH_WRITE_MASK_ALL,
			D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_LESS_EQUAL);	// LessEqual（近ければ描画される）
		PSO& SetStencilState(bool enable,
			D3D12_DEPTH_STENCILOP_DESC front = D3D12_DEPTH_STENCILOP_DESC(
				D3D12_STENCIL_OP_KEEP,
				D3D12_STENCIL_OP_KEEP,
				D3D12_STENCIL_OP_KEEP,
				D3D12_COMPARISON_FUNC_ALWAYS),	// 何もしない設定
			D3D12_DEPTH_STENCILOP_DESC back = D3D12_DEPTH_STENCILOP_DESC(
				D3D12_STENCIL_OP_KEEP,
				D3D12_STENCIL_OP_KEEP,
				D3D12_STENCIL_OP_KEEP,
				D3D12_COMPARISON_FUNC_ALWAYS));
		PSO& SetDSVFormat(DXGI_FORMAT format);
		PSO& SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void Build();

		// PSOの設定をコピーする関数
		PSO& Copy(const PSO& source);
		// PSOの設定をコピーする関数（ルートは別のものを指定するver）
		PSO& Copy(const PSO& source, ID3D12RootSignature* root);

		// ID3D12PipelineState型への暗黙の変換演算子をオーバーロード
		operator ID3D12PipelineState* () {
			return state_.Get();
		}
		// PipelineStateを受け取る関数（あとで消す）
		ID3D12PipelineState* GetState() { return state_.Get(); }

		// コピー用の処理
		Type GetType() const { return type_; }
		// コピー用の詳細
		Desc GetDesc() const { return desc_; }

	private: // ** メンバ変数 ** //
		// グラフィックパイプラインの状態を定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state_ = nullptr;
		// PSOの種類
		Type type_ = Type::Vertex;
		// PSOの詳細
		Desc desc_;

		// ディレクトリパス
		const std::string kDirectoryPath = "resources/system/shaders/";


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// デフォルトの生成
		/// </summary>
		D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
		D3D12_BLEND_DESC CreateBlendState();
		D3D12_RASTERIZER_DESC CreateRasterizerState();
		//IDxcBlob* CreateVertexShader();
		//IDxcBlob* CreatePixelShader();
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	};
}