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
		/// <param name="r">0 ... Wire、1 ... Fill</param>
		/// <param name="vs">0 ... nullptr、1 ... Object3d、2 ... ShadowMap</param>
		/// <param name="ps">0 ... nullptr、1 ... Object3d</param>
		void Initialize(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc, UINT r, UINT vs, UINT ps);

		/// <summary>
		/// シャドウマップ専用のPSO初期化用
		/// </summary>
		void InitializeForShadow(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc);

	private: // ** プライベートな関数 ** //

		D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
		D3D12_BLEND_DESC CreateBlendState();
		D3D12_RASTERIZER_DESC CreateRasterizerState(UINT r);
		IDxcBlob* CreateVertexShader(DXC* dxc, UINT vs);
		IDxcBlob* CreatePixelShader(DXC* dxc, UINT ps);
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}