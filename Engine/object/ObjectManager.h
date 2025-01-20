#pragma once
#include "core/Camera.h"
#include "core/Particle.h"
#include "core/Terrain.h"
#include "core/light/DirectionLight.h"
#include "core/light/PointLight.h"

#include "utility/PtrManager.h"

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
		void Update(Base::RendererManager* manager);

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(IObject* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(IObject* ptr);


	private: // メンバ変数

		// オブジェクトのリスト
		Utility::PtrManager<IObject*> objects_;
		// カウンター
		int objectCount_ = 0;


	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IObject*> debugPris;

	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
}