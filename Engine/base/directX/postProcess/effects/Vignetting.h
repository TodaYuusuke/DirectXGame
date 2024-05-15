#pragma once
#include "../IPostProcess.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ビネット
	/// </summary>
	class Vignetting final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 強度（現在機能しない）
		float intensity;


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