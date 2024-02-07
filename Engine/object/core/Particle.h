#pragma once
#include "../IObject.h"
#include "primitive/IPrimitive.h"

namespace LWP::Object {
	// 構造体
	struct ParticleData {
		LWP::Object::WorldTransform wtf;
		LWP::Math::Vector3 velocity;
		int elapsedFrame = 0;
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

		// パーティクルを追加
		void Add(int value);
		// デバッグ用GUI
		void DebugGUI() override;

		// パーティクルの形状を設定
		LWP::Primitive::IPrimitive* primitive = nullptr;
		// 初期化関数ポインタ（）
		std::function<ParticleData(LWP::Primitive::IPrimitive*)> initFunction = nullptr;
		// 更新ポインタ（返り値：このWTFを削除するかのフラグ）
		std::function<bool(ParticleData*)> updateFunction = nullptr;


	private: // ** メンバ変数 ** //

		// パーティクルのトランスフォーム
		std::vector<ParticleData> data;
	};
}