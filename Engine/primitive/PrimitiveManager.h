#pragma once
#include "2d/2dList.h"
#include "3d/3dList.h"

#include "utility/other/PtrManager.h"
#include "utility/other/Singleton.h"
#include "base/directX/resource/data/StructuredBuffer.h"
#include "base/directX/renderer/BufferGroup.h"

#include <vector>
#include <map>

namespace LWP::Primitive {
	// 描画に使うデータ
	struct PlaneBuffers {
		Base::StructuredBuffer<Base::VertexStruct> vertices;		// 頂点
		Base::StructuredBuffer<Base::WTFStruct> wtf;				// ワールドトランスフォーム
		Base::StructuredBuffer<Base::MaterialStruct> materials;		// マテリアル
		Base::StructuredBuffer<int32_t> zSort;

		// インスタンス数
		int count;

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PlaneBuffers() = delete;
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PlaneBuffers(int bufferSize);
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
		void SetPlanePtr(IPlane* ptr);
		void SetPri3DPtr(IPrimitive3D* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePlanePtr(IPlane* ptr) { planes_.DeletePtr(ptr); }
		void DeletePri3DPtr(IPrimitive3D* ptr) { pris3D_.DeletePtr(ptr); }
		
		// Plane系の描画に使うバッファーを送る関数
		PlaneBuffers* GetSpriteBuffer() { return &planeBuffers_[PlaneRenderType::Sprite]; }
		PlaneBuffers* GetBillboardBuffer() { return &planeBuffers_[PlaneRenderType::Billboard]; }

		// ビルボードの種類のバッファを返す関数
		D3D12_GPU_DESCRIPTOR_HANDLE GetTypeBuffer() { return type_.GetGPUView(); }
		// ストレッチビルボードの速度バッファを返す関数
		D3D12_GPU_DESCRIPTOR_HANDLE GetVelocitiesBuffer() { return velocities_.GetGPUView(); }

	private: // ** プライベートなメンバ関数 ** //

		// Plane用のDebugGUI
		void DebugGUIPlane();
		// 3DPrimitive用のDebugGUI
		void DebugGUI3DPrimitive();

	private: // ** メンバ変数 ** //

		// カウンター
		int primitiveCount_ = 0;

		enum PlaneRenderType {
			Sprite,
			Billboard,
			Count
		};

		// Planeのポインタリスト
		Utility::PtrManager<IPlane*> planes_;
		PlaneBuffers planeBuffers_[PlaneRenderType::Count];

		Utility::PtrManager<IPrimitive3D*> pris3D_;

		// ビルボードの種類（シェーダー内で描画切り替え用）
		Base::StructuredBuffer<int32_t> type_;
		// ストレッチビルボード用の速度
		Base::StructuredBuffer<Math::Vector3> velocities_;


		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IPrimitive*> debugPris;
		std::vector<IPrimitive3D*> debug3DPris;
	};
}