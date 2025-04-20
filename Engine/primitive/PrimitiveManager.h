#pragma once
#include "2d/2dList.h"
#include "3d/3dList.h"

#include "IPrimitive.h"
#include "utility/PtrManager.h"

#include "utility/Singleton.h"
#include <vector>
#include <map>

namespace LWP::Primitive {
	/// <summary>
	/// 描画用の形状をすべて管理するクラス
	/// </summary>
	class Manager : public Utility::ISingleton<Manager> {
		friend class Utility::ISingleton<Manager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		Manager() = default;

	public: // メンバ関数

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw(Base::RendererManager* manager);

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(IPrimitive* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(IPrimitive* ptr);


	private: // メンバ変数

		// 形状のリスト
		Utility::PtrManager<IPrimitive*> primitives_;
		// カウンター
		int primitiveCount_ = 0;
		

	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IPrimitive*> debugPris;

	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
}