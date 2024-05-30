#pragma once
#include "../IPostProcess.h"

#include "base/directX/utility/RootSignature.h"
#include "base/directX/utility/PSO.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ブルーム
	/// </summary>
	class Bloom final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 強度
		float intensity;


	private: // ** メンバ変数 ** //

		Base::ConstantBuffer<float> buffer_;
		// ブルームをかけるのに一時的に必要なデータ
		struct TempData {
			Base::RenderResource rr;
			Base::RootSignature root;
			Base::PSO pso;
		};
		TempData brightnessFilter;	// 輝度抽出
		TempData gaussX;	// Xガウス
		PSO gaussY;	// Yガウス

	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 更新
		void Update() override;

		// シェーダーでincludeする処理を書き込む
		void WriteBinding(std::ofstream* stream, RootSignature* root, int* i) override;
		// シェーダー内の処理を書き込む
		void WriteProcess(std::ofstream* stream) override;
		// commandListにBind情報を指示
		void BindCommand(ID3D12GraphicsCommandList* list, int* offset) override;
		// 事前に行わなければいけない処理をここにまとめる
		void PreCommand(ID3D12GraphicsCommandList* list, Base::RenderResource* target) override;
		// ImGui
		void DebugGUI() override;

	};
}