#pragma once
#include "../IObject.h"
#include "resources/model/StaticModel.h"

#if DEMO
#include "primitive/3d/Cube.h"
#include "resources/model/RigidModel.h"
#endif

#include "utility/PtrManager.h"

#include <map>

namespace LWP::Object {
	// 前方宣言
	class Terrain;

	// 地形の当たり判定
	namespace TerrainCollider {
		// ** 点 ** //
		class Point {
		public: // ** メンバ関数 ** //
			// 初期化関数
			void Init(Terrain* terrain, TransformQuat* tf);
			// デストラクタ
			~Point();

		public: // ** パブリックなメンバ変数 ** //
			Math::Vector3 offset;
			TransformQuat* wtf;
			bool preFrameHit = false;

		private: // ** メンバ変数 ** //
			// Terrainのポインタを保持（デストラクタ排除用）
			Terrain* terrain_;
		};
	}

	/// <summary>
	/// 地形クラス
	/// </summary>
	class Terrain final : public IObject {
	public: // ** メンバ関数 ** //

		// デフォルトコンストラクタ
		Terrain();

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// 地形に使うモデルデータを読み込む
		void LoadModel(std::string filePath, const TransformQuat& wtf);

		// 点の当たり判定をセット（※ユーザー呼び出し不要）
		void SetPointCollider(TerrainCollider::Point* p) { points_.SetPointer(p); }
		// 点の当たり判定を削除（※ユーザー呼び出し不要）
		void DeletePointCollider(TerrainCollider::Point* p) { points_.DeletePointer(p); }

		// デバッグ用GUI
		void DebugGUI() override;


	private: // ** メンバ定数 ** //

		// 空間分割レベル
		const uint32_t kSubdivision_ = 4;


	private: // ** メンバ変数 ** //

		// 地形のモデル
		Resource::StaticModel model_;
		
		// ** 当たり判定（地形用） ** //
		struct Polygon {
			Math::Vector3 pos[3];	// 3点
			Math::Vector3 normal;	// 向いている法線
		};
		// モートン序列番号（8分木空間分割）ごとにリスト化されたポリゴンのマップ
		std::map<int, std::list<Polygon>> polygonMap_;
		Math::Vector3 min_;	// モートン序列の生成に使うため保持
		Math::Vector3 max_;	
		float cellSize_;

		// 地形との当たり判定（点）
		Utility::PtrManager<TerrainCollider::Point*> points_;

		// 草を生やす座標
		struct Grass {
			Math::Vector3 patchPosition;
			Math::Vector3 groundNormal;
			float height;
		};
		//Base::RWStructuredBuffer<Math::Vector3> grassPositions_;
		

	private: // ** デバッグ用 ** //
		// ImGui用変数
		int currentItem = 0;
#if DEMO
		// 8分木の空間表示
		std::vector<Resource::RigidModel> cubes_;
		// ヒットしていた三角形の頂点に目印表示用
		Resource::RigidModel triangleCube_[4];
#endif

	private: // ** プライベートなメンバ関数 ** //

		// ビットを3ビットごとに広げる関数
		DWORD BitSeparate(BYTE n);
		// 点のモートン序列番号を求める関数
		DWORD GetMortonOrder(const Math::Vector3& point);
		// ポリゴンの所属する空間番号を求める関数
		int GetMortonNumber(Math::Vector3 point);
		int GetMortonNumber(const Terrain::Polygon& p);

		// その空間レベルまでの要素数を返す関数
		int GetSpaceLevelObjectsSum(const int& spaceLevel);

		// 当たり判定チェック

	};
}
