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

	
	// パイプラインの詳細設定
	//enum class FillMode : UINT {
	//	Wire = 0,
	//	Fill = 1
	//};
	struct RasterizerState {
		enum class CullMode : UINT {
			None = D3D12_CULL_MODE_NONE,	// カリングしない
			Front = D3D12_CULL_MODE_FRONT,	// 前面を削除
			Back = D3D12_CULL_MODE_BACK		// 背面を削除
		}cullMode = CullMode::Back;
		enum class FillMode : UINT {
			WireFrame = D3D12_FILL_MODE_WIREFRAME,	// 汎用
			Solid = D3D12_FILL_MODE_SOLID	// シャドウマップ用
		}fillMode = FillMode::Solid;
	};

	enum class DepthFormat : UINT {
		D24_UNORM_S8_UINT = 0,	// 汎用
		D32_FLOAT = 1	// シャドウマップ用
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
		void Initialize(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc, RasterizerState rState, UINT vs, UINT ps, DepthFormat df);

		/// <summary>
		/// シャドウマップ専用のPSO初期化用
		/// </summary>
		void InitializeForShadow(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc);

	private: // ** プライベートな関数 ** //

		D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
		D3D12_BLEND_DESC CreateBlendState();
		D3D12_RASTERIZER_DESC CreateRasterizerState(RasterizerState rState);
		IDxcBlob* CreateVertexShader(DXC* dxc, UINT vs);
		IDxcBlob* CreatePixelShader(DXC* dxc, UINT ps);
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

		/// <summary>
		/// シェーダーのコンパイル関数
		/// </summary>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
	};
}