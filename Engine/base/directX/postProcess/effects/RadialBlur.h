#pragma once
#include "../IPostProcess.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ラジアルブラー
	/// </summary>
	class RadialBlur final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 中心座標
		LWP::Math::Vector2 center = { 0.5f,0.5f };
		// ブラー強度
		float blurWidth = 0.01f;

	private: // ** メンバ変数 ** //
		struct Data {
			LWP::Math::Vector2 center;
			float blurWidth;
		};
		Base::ConstantBuffer<Data> buffer_;

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