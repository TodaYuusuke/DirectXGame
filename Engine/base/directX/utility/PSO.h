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
	public: // **　列挙子 ** //
		enum class Type {
			Vertex,
			Mesh
		};


	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		PSO& Init(ID3D12RootSignature* root, DXC* dxc, Type type = Type::Vertex);
		PSO& SetInputLayout();
		PSO& SetBlendState();
		PSO& SetRasterizerState(
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID);
		PSO& SetMeshShader(std::string filePath);
		PSO& SetVertexShader(std::string filePath);
		PSO& SetPixelShader(std::string filePath);
		PSO& SetDepthStencilState(bool enable);
		PSO& SetDSVFormat(DXGI_FORMAT format);
		PSO& SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void Build(ID3D12Device2* device);

		// PipelineStateを受け取る関数
		ID3D12PipelineState* GetState() { return state_.Get(); }


	private: // ** メンバ変数 ** //
		// グラフィックパイプラインの状態を定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state_ = nullptr;
		// PSOの種類
		Type type_;
		// PSOの詳細
		union Desc {
			D3D12_GRAPHICS_PIPELINE_STATE_DESC vertex;
			D3DX12_MESH_SHADER_PIPELINE_STATE_DESC mesh;

			Desc() {};
			~Desc() {};
		}desc_;
		// DXCのポインタ
		DXC* dxc_ = nullptr;


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