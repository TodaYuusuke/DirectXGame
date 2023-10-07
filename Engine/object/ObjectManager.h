#pragma once
#include "core/CoreList.h"
#include "user/UserList.h"
#include <list>


namespace LWP::Object {
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
		void Draw();

		
		/// <summary>
		/// インスタンスを登録する
		/// </summary>
		/// <param name="object">登録するオブジェクト</param>
		template <IsIObject TObject>
		inline TObject* CreateInstance() {
			TObject* newObject = new TObject();
			objects_.push_back(newObject);
			return newObject;
		}

	private: // メンバ変数

		// オブジェクトのリスト
		std::list<IObject*> objects_;
	};
}