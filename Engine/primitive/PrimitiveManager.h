#pragma once
//#include "../base/CommandManager.h"
#include "2d/2dList.h"
#include "3d/3dList.h"

#include "IPrimitive.h"

#include <map>

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

			// typeid を使用して型情報を取得
			const std::type_info& typeInfo = typeid(newPrimitive);
			// type_info オブジェクトからクラス名を取得
			std::string className = typeInfo.name();


			// カウントのマップから数を測定し、デフォルトの名前を登録
			newPrimitive->name = className + std::to_string(primitiveCountMap_[className]++);

			// リストに登録
			primitives_.push_back(newPrimitive);
			return newPrimitive;
		}

	private: // メンバ変数

		// 形状のリスト
		std::list<IPrimitive*> primitives_;

		// 形状のインスタンスカウント
		std::map<std::string, uint16_t> primitiveCountMap_;
	};
}