#pragma once
#include "RootSignature.h"
#include "PSO.h"

namespace LWP::Base {
	/// <summary>
	/// シェーダークラス
	/// </summary>
	class Shader final {
	public: // **　メンバ関数 ** //

		// 生成処理
		void Init(std::string)

		// ビルド
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