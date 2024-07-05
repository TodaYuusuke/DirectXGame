#pragma once
#include "../IPostProcess.h"

#include "math/matrix/Matrix4x4.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// アウトライン
	/// </summary>
	class OutLine final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// 強度
		//float intensity;


	private: // ** メンバ変数 ** //
		struct Data {
			//float intensity;
			Math::Matrix4x4 projectionInverse;	// カメラのプロジェクション行列の逆行列
		};
		
		Base::ConstantBuffer<Data> buffer_;

	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 更新
		void Update() override;

		// カメラのプロジェクション行列の逆行列をセットするための関数
		void SetProjectionInverse(const Math::Matrix4x4& m) { buffer_.data_->projectionInverse = m; }

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