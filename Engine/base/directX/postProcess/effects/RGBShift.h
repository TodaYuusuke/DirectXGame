#pragma once
#include "../IPostProcess.h"
#include "math/vector/Vector2.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// RGBずらし
	/// </summary>
	class RGBShift final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 各チャンネルのずらし量
		Math::Vector2 shiftR;
		Math::Vector2 shiftG;
		Math::Vector2 shiftB;


	private: // ** メンバ変数 ** //

		struct Data {
			Math::Vector2 shiftR;
			Math::Vector2 shiftG;
			Math::Vector2 shiftB;
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
