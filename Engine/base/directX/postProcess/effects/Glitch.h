#pragma once
#include "../IPostProcess.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// グリッチエフェクト
	/// </summary>
	class Glitch final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 強度 (0.0 - 1.0)
		float strength;


	private: // ** メンバ変数 ** //

		Base::ConstantBuffer<float> buffer_;

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
		// ImGui
		void DebugGUI() override;

	};
}
