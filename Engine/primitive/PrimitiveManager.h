#pragma once
//#include "../base/CommandManager.h"
#include "2d/2dList.h"
#include "3d/3dList.h"

#include "IPrimitive.h"

namespace LWP::Primitive {
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
		//void Update();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(Base::CommandManager* manager);


		/// <summary>
		/// インスタンスを登録する
		/// </summary>
		/// <param name="TPrimitive">登録する形状</param>
		template <IsIPrimitive TPrimitive>
		inline TPrimitive* CreateInstance(Base::CommandManager* manager) {
			TPrimitive* newPrimitive = new TPrimitive(manager);
			newPrimitive->CreateVertices();
			newPrimitive->CreateIndexes();
			primitives_.push_back(newPrimitive);
			return newPrimitive;
		}

	private: // メンバ変数

		// オブジェクトのリスト
		std::list<IPrimitive*> primitives_;
	};
}