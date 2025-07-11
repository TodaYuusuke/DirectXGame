#pragma once
#include "2d/2dList.h"
//#include "3d/3dList.h"

#include "utility/PtrManager.h"
#include "utility/Singleton.h"

#include "base/directX/resource/data/StructuredBuffer.h"
#include "base/directX/renderer/ResourceStruct.h"

#include <vector>
#include <map>

namespace LWP::Primitive {
	// スプライトやビルボードの描画に使うデータ
	struct PlaneBuffers {
		Base::StructuredBuffer<Base::VertexStruct> vertices;		// 頂点
		Base::StructuredBuffer<Base::WTFStruct> wtf;				// ワールドトランスフォーム
		Base::StructuredBuffer<Base::MaterialStruct> materials;		// マテリアル

		// インスタンス数
		int count;

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PlaneBuffers();

		/// <summary>
		/// リセットを呼び出す関数
		/// </summary>
		void Reset();
	};

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
		void SetSpritePtr(IPlane* ptr);
		void SetBillboard3DPtr(IPlane* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeleteSpritePtr(IPlane* ptr) { sprites_.DeletePtr(ptr); }
		void DeleteBillboard3DPtr(IPlane* ptr) { billboards3D_.DeletePtr(ptr); }

		// Plane系の描画に使うバッファーを送る関数
		PlaneBuffers* GetSpriteBuffer() { return &spriteBuffers_; }
		// Plane系の描画に使うバッファーを送る関数
		PlaneBuffers* GetBillboard3DBuffer() { return &billboard3DBuffers_; }


	private: // ** メンバ変数 ** //

		// 形状のリスト
		//Utility::PtrManager<IPrimitive*> primitives_;
		// カウンター
		int primitiveCount_ = 0;
		

		enum class Type {
			Sprite, Billboard,
		};
		// 全Spriteのポインタリスト
		Utility::PtrManager<IPlane*> sprites_;
		PlaneBuffers spriteBuffers_;

		// Billboard3Dのポインタリスト
		Utility::PtrManager<IPlane*> billboards3D_;
		PlaneBuffers billboard3DBuffers_;

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