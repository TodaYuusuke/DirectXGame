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
	public: // ** メンバ関数 ** //
		// デストラクタ
		~Particle();

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// パーティクルの形状を指定
		template<IsIPrimitive T>
		void SetPrimitive() { 
			if (primitive) { delete primitive; }
			primitive = new T(); 
		}
		// トランスフォームをいじる
		Object::WorldTransform* Transform() { return &primitive->transform; }
		// パーティクルを追加
		void Add(int value);
		// デバッグ用GUI
		void DebugGUI() override;

		// 初期化関数ポインタ（）
		std::function<ParticleData(LWP::Primitive::IPrimitive*)> initFunction = nullptr;
		// 更新ポインタ（返り値：このWTFを削除するかのフラグ）
		std::function<bool(ParticleData*)> updateFunction = nullptr;


	private: // ** メンバ変数 ** //

		// パーティクルの形状
		LWP::Primitive::IPrimitive* primitive = nullptr;
		// パーティクルのトランスフォーム
		std::vector<ParticleData> data;
	};
}