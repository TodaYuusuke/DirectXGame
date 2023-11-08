#pragma once
#include "core/CoreList.h"

#include <vector>
#include <map>
#include <typeinfo>

namespace LWP::Object {
	/// <summary>
	/// システム用のオブジェクトをまとめて管理するクラス
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
		void Update(Base::CommandManager* manager);

		
		/// <summary>
		/// インスタンスを登録する
		/// </summary>
		/// <param name="object">登録するオブジェクト</param>
		template <IsIObject TObject>
		inline TObject* CreateInstance() {
			TObject* newObject = new TObject();

			// typeid を使用して型情報を取得
			const std::type_info& typeInfo = typeid(TObject);
			// type_info オブジェクトからクラス名を取得
			std::string className = typeInfo.name();
			// 名前空間部分を削除（LWP::Object::）
			className = className.erase(0, 19);

			// カウントのマップから数を測定し、デフォルトの名前を登録
			if (!objectCountMap_.count(className)) {
				// 存在しない場合のみ0で初期化
				objectCountMap_[className] = 0;
			}
			newObject->name = className + "_" + std::to_string(objectCountMap_[className]++);

			objects_.push_back(newObject);
			return newObject;
		}

	private: // メンバ変数

		// オブジェクトのリスト
		std::vector<IObject*> objects_;
		// インスタンスカウント用マップ
		std::map<std::string, int> objectCountMap_;

#if _DEBUG
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
#endif
	};
}