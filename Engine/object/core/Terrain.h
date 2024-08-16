#pragma once
#include "../IObject.h"
#include "resources/model/StaticModel.h"

#if DEMO
#include "primitive/3d/Cube.h"
#include "resources/model/RigidModel.h"
#endif

#include <map>

namespace LWP::Object {
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

		// ** 当たり判定（点） ** //
		struct Point {
			Math::Vector3 offset;
			TransformQuat* wtf;
			float radius;	// 草を潰す半径
			bool preFrameHit = false;
		};
		// 地形と検証する当たり判定を追加する関数
		Point* SetNewCollider(Math::Vector3 offset, TransformQuat* wtf);

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
		std::vector<Point> points_;

		// 草を生やす座標
		struct Grass {
			Math::Vector3 patchPosition;
			Math::Vector3 groundNormal;
			float height;
		};
		Base::StructuredBuffer<Math::Vector3> grassPositions_;
		

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
