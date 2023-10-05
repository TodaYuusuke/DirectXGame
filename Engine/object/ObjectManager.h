#pragma once
#include "IObject.h"
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
		/// オブジェクトのインスタンスを登録する
		/// </summary>
		/// <param name="object">登録するオブジェクト</param>
		template <IsIObject TObject>
		inline TObject* CreateObjectInstance() {
			TObject* newObject = new TObject();
			objects.push_back(newObject);
			return newObject;
		}

	private: // メンバ変数

		// オブジェクトのリスト
		std::list<IObject*> objects;
	};
}