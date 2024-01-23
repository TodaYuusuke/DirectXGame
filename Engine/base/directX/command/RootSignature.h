#pragma once
#include "../GPUDevice.h"

#include <vector>

namespace LWP::Base {
	// シェーダー上での可視性
	enum ShaderVisibility {
		SV_Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
		SV_Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
		SV_All = D3D12_SHADER_VISIBILITY_ALL
	};

	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	class RootSignature final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		RootSignature& AddCBVParameter(int registerNum, ShaderVisibility visibility);
		RootSignature& AddTableParameter(int registerNum, ShaderVisibility visibility, int space = 0, UINT maxSize = 0);
		RootSignature& AddSampler(int registerNum, ShaderVisibility visibility = SV_Pixel,
			D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// フィルター（default:バイオリニアフィルタ）
			D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_NEVER,	// 比較関数（default:比較しない）
			D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// 範囲外をどうするかの設定
			D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// 範囲外をどうするかの設定
			D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP	// 範囲外をどうするかの設定
			);
		void Build(ID3D12Device* device);

		// RootSignatureを受け取る関数
		ID3D12RootSignature* GetRoot() { return root_.Get(); }

	private: // ** メンバ変数 ** //
		// ルートシグネチャ本体
		Microsoft::WRL::ComPtr<ID3D12RootSignature> root_ = nullptr;

		// 詳細設定
		D3D12_ROOT_SIGNATURE_DESC desc_;

		// パラメータの可変長配列
		std::vector<D3D12_ROOT_PARAMETER> parameters_;
		// サンプラーの可変長配列
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers_;

		// Structedのデフォルト設定
		D3D12_DESCRIPTOR_RANGE defaultTableDesc_ = {
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// TableなのでSRVを使う
			1,	// 数は1つ
			0,	// ここは可変
			0,	// スペースは基本0
			D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND	// Offsetを自動計算
		};
	};
}