#pragma once
#include "../utility/RootSignature.h"
#include "../utility/PSO.h"

#include "effects/Bloom.h"
#include "effects/OutLine.h"
#include "effects/GrayScale.h"
#include "effects/Vignetting.h"
#include "effects/RadialBlur.h"
#include "effects/RGBShift.h"
#include "effects/Glitch.h"

#include "base/directX/resource/data/ConstantBuffer.h"
#include <fstream>

namespace LWP::Base {
	/// <summary>
	/// ポストプロセスの共通パラメータ構造体
	/// </summary>
	struct PostProcessParameter {
		float time;
		int rWidth;
		int rHeight;
		int padding;
	};

	/// <summary>
	/// ポストプロセス管理クラス
	/// <para>HLSLコードをバイナリ形式のGPUシェーダーに変換する</para>
	/// </summary>
	class PostProcessor final {
	public: // ** パブリックなメンバ変数 ** //

		// そもそも行うかフラグ
		bool use = false;

		PostProcess::Bloom bloom;	// ブルーム
		PostProcess::OutLine outLine;	// アウトライン
		PostProcess::GrayScale grayScale;	// グレースケール
		PostProcess::RGBShift rgbShift;	// RGBずらし
		PostProcess::Glitch glitch;		// グリッチ
		PostProcess::RadialBlur radialBlur;	// ラジアルブラー
		PostProcess::Vignetting vignetting;	// ビネット
		// ここにどんどん種類を増やしていく


	public: // ** メンバ関数 ** //

		// 初期化
		void Init();
		// 更新
		void Update();

		// パラメーターからシェーダーファイルを生成しPSOを生成する関数
		void CreateShaderFile();
		// 既存のシェーダーファイルからPSOを生成する関数
		void CreatePSO(std::string filePath);

		// 描画する前に行わなければならない処理
		void PreCommands(ID3D12GraphicsCommandList* list, Base::RenderResource* target);
		// コマンドリストにView当をセットする関数
		void SetCommands(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI();

		// 共通化した可変長配列を返す関数
		std::vector<PostProcess::IPostProcess*> GetAllProcess();


	private: // ** メンバ変数 ** //

		// RootSignature
		RootSignature root_;
		// PSO
		PSO pso_;

		// 共通パラメータ
		Base::ConstantBuffer<PostProcessParameter> commonBuffer_;

		// 処理を行うポストプロセスたち
		std::vector<PostProcess::IPostProcess*> processes_;
	};
}
