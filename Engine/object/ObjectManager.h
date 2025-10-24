#pragma once
#include "core/Camera.h"
#include "core/Particle.h"
#include "core/SpriteParticle.h"
#include "core/GPUParticle.h"
#include "core/light/DirectionLight.h"
#include "core/light/PointLight.h"

#include "utility/other/PtrManager.h"

#include <vector>
#include <map>
#include <typeinfo>

namespace LWP::Object {
	/// <summary>
	/// システム用のオブジェクトをまとめて管理するクラス
	/// </summary>
	class Manager : public Utility::ISingleton<Manager> {
		friend class Utility::ISingleton<Manager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		Manager() = default;

	public:	// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// メインカメラにセットする関数
		/// </summary>
		void SetMainCamera(Camera* camera) { mainCamera_ = camera; }
		/// <summary>
		/// メインカメラを取得する関数
		/// </summary>
		Camera* GetMainCamera() { return mainCamera_; }

		// 配列にポインタを登録する関数
		void SetPtr(Camera* ptr);
		void SetPtr(Particle* ptr);
		void SetPtr(SpriteParticle* ptr);
		void SetPtr(GPUParticle* ptr);
		void SetPtr(PointLight* ptr);
		// 配列からポインタを削除する関数
		void DeletePtr(Camera* ptr) { cameras_.DeletePtr(ptr); }
		void DeletePtr(Particle* ptr) { particle_.DeletePtr(ptr); }
		void DeletePtr(SpriteParticle* ptr) { spriteParticle_.DeletePtr(ptr); }
		void DeletePtr(GPUParticle* ptr) { gpuParticle_.DeletePtr(ptr); }
		void DeletePtr(PointLight* ptr) { pointLight_.DeletePtr(ptr); }
		// 配列を描画処理に渡す関数
		const std::list<Camera*>& GetCameras() const { return cameras_.list; }
		const std::list<Particle*>& GetParticles() const { return particle_.list; }
		const std::list<GPUParticle*>& GetGPUParticles() const { return gpuParticle_.list; }
		const std::list<PointLight*>& GetPointLights() const { return pointLight_.list; }

		/// <summary>
		/// 平行光源のポインタを返す関数
		/// </summary>
		DirectionLight* GetDirLight() { return &directionLight_; }

	private: // メンバ変数

		enum class Type {
			Camera,
			Particle,
			SpriteParticle,
			GPUParticle,
			Terrain,
			DirectionLight,
			PointLight
		};
		// オブジェクトのリスト
		Utility::PtrManager<Camera*> cameras_;
		Utility::PtrManager<Particle*> particle_;
		Utility::PtrManager<SpriteParticle*> spriteParticle_;
		Utility::PtrManager<GPUParticle*> gpuParticle_;
		DirectionLight directionLight_;		// インスタンスは必ず１つなので直で持つ
		Utility::PtrManager<PointLight*> pointLight_;
		// カウンター
		int objectCount_ = 0;

		// メインカメラのポインタ
		LWP::Object::Camera* mainCamera_ = nullptr;
		// ライト系のメタデータ用バッファー
		struct LightMetadata {
			// 点光源の数
			int pointLightCount = 0;
		};
		LWP::Base::ConstantBuffer<LightMetadata> lightMetadataBuffer_;

	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IObject*> debugObjs_;

	public:

		// ライト系のメタデータを取得する関数
		const LWP::Base::ConstantBuffer<LightMetadata>& GetLightMetadataBuffer() const { return lightMetadataBuffer_; }

		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
}