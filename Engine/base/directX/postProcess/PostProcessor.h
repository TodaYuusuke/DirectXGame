#pragma once
#include "../utility/RootSignature.h"
#include "../utility/PSO.h"


#include "effects/GrayScale.h"

#include <fstream>

namespace LWP::Base {
	/// <summary>
	/// ポストプロセス管理クラス
	/// <para>HLSLコードをバイナリ形式のGPUシェーダーに変換する</para>
	/// </summary>
	class PostProcessor final {
	public: // ** パブリックなメンバ変数 ** //

		PostProcess::GrayScale grayScale;	// グレースケール
		// ここにどんどん種類を増やしていく


	public: // ** メンバ関数 ** //

		// シェーダーファイルを生成する関数
		//void CreateShaderFile();
		// 既存のシェーダーファイルからPSOを生成する関数
		void CreatePSO(std::string filePath);

		// RootSignatureを返す関数
		RootSignature* GetRoot() { return &root_; }
		// PSOを返す関数
		PSO* GetPSO() { return &pso_; }


	private: // ** メンバ変数 ** //

		// RootSignature
		RootSignature root_;
		// PSO
		PSO pso_;
	};
}