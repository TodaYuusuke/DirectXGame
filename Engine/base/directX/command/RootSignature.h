#pragma once
#include "../GPUDevice.h"

#include <string>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

namespace LWP::Base {
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	class RootSignature final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		RootSignature& Init();
		RootSignature& AddParameter();
		RootSignature& AddSampler();
		void Build();

		// RootSignatureを受け取る関数
		ID3D12RootSignature* GetRoot() { return root_.Get(); }

	private: // ** メンバ変数 ** //
		// ルートシグネチャ本体
		Microsoft::WRL::ComPtr<ID3D12RootSignature> root_ = nullptr;


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

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}