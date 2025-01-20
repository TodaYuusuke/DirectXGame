#pragma once
#include "GPUDevice.h"
#include "DXC.h"

#include <DirectXTex/d3dx12.h>

#include <string>

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
		// ブレンドモード
		enum class BlendMode {
			Normal,
			Add,
			Subtract,
			Multiply,
			Screen
		};


	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		PSO& Init(ID3D12RootSignature* root, Type type = Type::Vertex);
		PSO& SetInputLayout();
		PSO& SetBlendState(BlendMode mode = BlendMode::Normal);
		PSO& SetRasterizerState(
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID);
		PSO& SetAmpShader(std::string filePath);
		PSO& SetMeshShader(std::string filePath);
		PSO& SetComputeShader(std::string filePath);
		PSO& SetVertexShader(std::string filePath);
		PSO& SetPixelShader(std::string filePath);
		PSO& SetDepthStencilState(bool enable);
		PSO& SetDSVFormat(DXGI_FORMAT format);
		PSO& SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void Build();

		// PipelineStateを受け取る関数
		ID3D12PipelineState* GetState() { return state_.Get(); }


	private: // ** メンバ変数 ** //
		// グラフィックパイプラインの状態を定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state_ = nullptr;
		// PSOの種類
		Type type_ = Type::Vertex;
		// PSOの詳細
		union Desc {
			D3D12_GRAPHICS_PIPELINE_STATE_DESC vertex{};
			D3D12_COMPUTE_PIPELINE_STATE_DESC compute;
			D3DX12_MESH_SHADER_PIPELINE_STATE_DESC mesh;

			Desc() {};
			~Desc() {};
		}desc_;


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