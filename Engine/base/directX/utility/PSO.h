#pragma once
#include "GPUDevice.h"
#include "DXC.h"

#include <string>

namespace LWP::Base {
	/// <summary>
	/// グラフィックスパイプライン
	/// </summary>
	class PSO final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		PSO& Init(ID3D12RootSignature* root, DXC* dxc);
		PSO& SetInputLayout();
		PSO& SetBlendState();
		PSO& SetRasterizerState(
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID);
		PSO& SetVertexShader(std::string filePath);
		PSO& SetPixelShader(std::string filePath);
		PSO& SetDepthStencilState(bool enable);
		PSO& SetDSVFormat(DXGI_FORMAT format);
		PSO& SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void Build(ID3D12Device* device);

		// PipelineStateを受け取る関数
		ID3D12PipelineState* GetState() { return state_.Get(); }


	private: // ** メンバ変数 ** //
		// グラフィックパイプラインの状態を定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state_ = nullptr;
		// PSOの詳細
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_{};
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