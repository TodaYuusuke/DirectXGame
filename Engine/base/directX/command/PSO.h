#pragma once
#include "../GPUDevice.h"

#include <string>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

namespace LWP::Base {
	//*** DirectXシェーダコンパイラ ***//
	// HLSLコードをバイナリ形式のGPUシェーダーに変換する
	struct DXC {
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;				// dxcの汎用オブジェクト
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;			// dxcのコンパイラオブジェクト
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;	// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
	};


	/// <summary>
	/// グラフィックスパイプライン
	/// </summary>
	class PSO final {
	public: // ** パブリックなメンバ変数 ** //
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state_;	// グラフィックパイプラインの状態を定義


	public: // **　メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc, int r);


	private: // ** プライベートな関数 ** //

		D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
		D3D12_BLEND_DESC CreateBlendState();
		D3D12_RASTERIZER_DESC CreateRasterizerState(int r);
		IDxcBlob* CreateVertexShader(DXC* dxc);
		IDxcBlob* CreatePixelShader(DXC* dxc);
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}