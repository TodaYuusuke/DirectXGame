#pragma once
#include "../IObject.h"
#include "primitive/IPrimitive.h"

namespace LWP::Object {
	// 構造体
	struct ParticleData {
		LWP::Object::WorldTransform wtf;
		LWP::Math::Vector3 velocity;
	};

	/// <summary>
	/// パーティクル
	/// </summary>
	class Particle final : public IObject {
	public: // ** パブリックなメンバ変数 ** //

		// ワールドトランスフォームの配列

	public: // ** メンバ関数 ** //

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;



		// 経過フレーム
		int elapsedFrame = 0;
		// パーティクルの形状を設定
		LWP::Primitive::IPrimitive* primitive = nullptr;
		// パーティクルのトランスフォーム
		std::vector<ParticleData> data;
		// 関数ポインタ（返り値：このWTFを削除するかのフラグ、ワールドトランスフォームのポインタ、経過フレーム、インデックス）
		std::function<bool(ParticleData*, int)> func = nullptr;


	private: // ** メンバ変数 ** //
	};
}