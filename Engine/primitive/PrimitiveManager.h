#pragma once
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
		void Update();

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
			const std::type_info& typeInfo = typeid(TPrimitive);
			// type_info オブジェクトからクラス名を取得
			std::string className = typeInfo.name();
			// 名前空間部分を削除（LWP::Primitive::）
			className = className.erase(0, 22);

			// カウントのマップから数を測定し、デフォルトの名前を登録
			if (!primitiveCountMap_.count(className)) {
				// 存在しない場合のみ0で初期化
				primitiveCountMap_[className] = 0;
			}
			newPrimitive->name = className + "_" + std::to_string(primitiveCountMap_[className]++);
			
			// リストに登録
			primitives_.push_back(newPrimitive);
			return newPrimitive;
		}

	private: // メンバ変数

		// 形状のリスト
		std::vector<IPrimitive*> primitives_;

		// 形状のインスタンスカウント
		std::map<std::string, int> primitiveCountMap_;
		
#if _DEBUG
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
#endif

	private: // ** プライベートな関数 ** //
		
		// ImGuiのListBox用関数
		static bool ListBoxGetter(void* data, int index, const char** output) {
			//IPrimitive* primitives = static_cast<IPrimitive*>(data);
			std::vector<IPrimitive*>* primitives = static_cast<std::vector<IPrimitive*>*>(data);
			*output = (*primitives)[index]->name.c_str();
			//*output = currentPrimitive.name.c_str();
			return true;
		}
	};
}