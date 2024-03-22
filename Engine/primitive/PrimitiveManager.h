#pragma once
#include "2d/2dList.h"
#include "3d/3dList.h"

#include "IPrimitive.h"
#include "utility/PtrManager.h"

#include <vector>
#include <map>

namespace LWP::Primitive {
	/// <summary>
	/// 描画用の形状をすべて管理するクラス
	/// </summary>
	class Manager {
	public: // メンバ関数

		// コンストラクタ
		Manager() = default;
		// デストラクタ
		~Manager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(Base::CommandManager* manager);


		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(IPrimitive* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(IPrimitive* ptr);


	private: // メンバ変数

		// 形状のリスト
		Utility::PtrManager<IPrimitive*> primitives_;

		// インスタンスカウント用マップ
		std::map<std::string, int> primitiveCountMap_;
		
#if DEMO
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IPrimitive> debugPris;
#endif
	};
}