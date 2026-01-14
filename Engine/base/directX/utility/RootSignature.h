#pragma once
#include "GPUDevice.h"

#include <vector>
#include <list>

namespace LWP::Base {
	// シェーダー上での可視性
	enum ShaderVisibility {
		SV_Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
		SV_Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
		SV_Mesh = D3D12_SHADER_VISIBILITY_MESH,
		SV_Amp = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
		SV_All = D3D12_SHADER_VISIBILITY_ALL
	};

	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	class RootSignature final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		RootSignature& AddCBVParameter(int registerNum, ShaderVisibility visibility);
		RootSignature& AddTableParameter(int registerNum, ShaderVisibility visibility, int space = 0, UINT maxSize = 0);
		RootSignature& AddUAVParameter(int registerNum, ShaderVisibility visibility, int space = 0, UINT maxSize = 0);
		RootSignature& AddUAVRWTexture(int registerNum, ShaderVisibility visibility, int space = 0);
		/// <param name="registerNum">レジスタ番号</param>
		/// <param name="visibility">シェーダー視認性</param>
		/// <param name="filter">フィルターの設定</param>
		/// <param name="func">比較関数</param>
		/// <param name="address">範囲外のUV座標をどうするか</param>
		/// <param name="borderColor">addressにBORDERが指定されている場合、境界の色指定</param>
		/// <param name="maxAnisotropy"></param>
		/// <returns></returns>
		RootSignature& AddSampler(int registerNum, ShaderVisibility visibility = SV_Pixel,
			D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// フィルター（default:バイオリニアフィルタ）
			D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_NEVER,	// 比較関数（default:比較しない）
			D3D12_TEXTURE_ADDRESS_MODE address = D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// 範囲外をどうするかの設定
			D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,	// ボーダーカラー（default:透明黒)
			int maxAnisotropy = 0
 		);
		void Build();

		// PSOの設定をコピーする関数
		RootSignature& Copy(const RootSignature& source);

		// ID3D12RootSignature型への暗黙の変換演算子をオーバーロード
		operator ID3D12RootSignature* () {
			return root_.Get();
		}

		// コピー用のゲッター
		D3D12_ROOT_SIGNATURE_DESC GetDesc() const { return desc_; }
		// パラメータのコピー用
		void CopyParameters(
			std::vector<D3D12_ROOT_PARAMETER>* para,
			std::list<D3D12_DESCRIPTOR_RANGE>* paraDesc,
			std::vector<D3D12_STATIC_SAMPLER_DESC>* samplers
		) const;


	private: // ** メンバ変数 ** //
		// ルートシグネチャ本体
		Microsoft::WRL::ComPtr<ID3D12RootSignature> root_ = nullptr;

		// 詳細設定
		D3D12_ROOT_SIGNATURE_DESC desc_;

		// パラメータの可変長配列
		std::vector<D3D12_ROOT_PARAMETER> parameters_;
		std::list<D3D12_DESCRIPTOR_RANGE> parametersDesc_;	// listにすることで途中で追加してもメモリが再割り当てされない
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