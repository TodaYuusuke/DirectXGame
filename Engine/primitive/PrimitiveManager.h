#pragma once
#include "2d/2dList.h"
//#include "3d/3dList.h"

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
		Manager();


	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// Debug用のImGUI
		/// </summary>
		void DebugGUI();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetSpritePtr(ISprite* ptr);
		void SetBillboardPtr(IBillboard2D* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeleteSpritePtr(ISprite* ptr) { sprites_.DeletePtr(ptr); }
		void DeleteBillboardPtr(IBillboard2D* ptr) { billboards_.DeletePtr(ptr); }

		// Plane系の描画に使うバッファーを送る関数
		SpriteBuffers* GetSpriteBuffer() { return &spriteBuffers_; }
		SpriteBuffers* GetBillboard2DBuffer() { return &billboardBuffers_; }

		// Zソートの結果のバッファを返す関数
		Base::StructuredBuffer<int32_t>* GetZSortBuffer() { return &zSort_; }


	private: // ** メンバ変数 ** //

		// 形状のリスト
		//Utility::PtrManager<IPrimitive*> primitives_;
		// カウンター
		int primitiveCount_ = 0;
		

		enum class Type {
			Sprite, Billboard,
		};
		// 全Spriteのポインタリスト
		Utility::PtrManager<ISprite*> sprites_;
		SpriteBuffers spriteBuffers_;

		// Billboardのポインタリスト
		Utility::PtrManager<IBillboard2D*> billboards_;
		SpriteBuffers billboardBuffers_;

		struct BillboardIndex {
			int index;
			float distance;
		};
		// ビルボードのZソート
		Base::StructuredBuffer<int32_t> zSort_;
		// ビルボードの種類
		//Base::StructuredBuffer<int32_t> type_;
		// ストレッチビルボード用の速度
		//Base::StructuredBuffer<int32_t> velocities_;


		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IPrimitive*> debugPris;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// スプライト系のDebugGUI
		/// </summary>
		void SpriteDebugGUI();
		/// <summary>
		/// ビルボード系のDebugGUI
		/// </summary>
		void BillboardDebugGUI();
	};
}