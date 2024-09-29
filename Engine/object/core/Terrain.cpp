#include "Terrain.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Object::TerrainCollider;
using namespace LWP::Math;
using namespace LWP::Resource;

void Point::Init(Terrain* terrain, TransformQuat* tf) {
	terrain_ = terrain;
	wtf = tf;
	terrain_->SetPointCollider(this);
}
Point::~Point() {
	terrain_->DeletePointCollider(this);
}

Terrain::Terrain() {
	Initialize();
}

// 初期化
void Terrain::Initialize() {
	//GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	//SRV* srv = System::engine->directXCommon_->GetSRV();
	////grassPositions_.Init(dev, srv);
}
// 更新
void Terrain::Update(Base::RendererManager* manager) {
	//if (!isActive) { return; }

	// 描画データ用のデータ登録
	manager;
	//manager->SetTerrainData(grassPositions_.GetSRVGPUView(), 0);

	// 当たり判定を検証
	for (Point* point : points_.list) {
		// wtfがnullなら処理しない
		if (!point->wtf) {
			continue;
		}

		// 座標を求める
		Vector3 pos = point->offset + point->wtf->GetWorldPosition();
		point->preFrameHit = false;

		// 検索するモートン番号
		const uint32_t kTargetMorton = GetMortonNumber(pos);
		if (kTargetMorton == -1) { continue; }	// -1だった場合早期終了
		uint32_t currentMorton = kTargetMorton;	// 現在のモートン番号（下位レベルから検証）
		uint32_t currentSpaceLevel = kSubdivision_;	// 現在の空間レベル（下位レベルから検証）

		while (true) {
			// 現在の空間内の全オブジェクトと検証
			for (const Polygon& p : polygonMap_[currentMorton]) {
				// 平面のパラメータ
				float distance = p.normal.x * p.pos[0].x + p.normal.y * p.pos[0].y + p.normal.z * p.pos[0].z;
				// 垂直の場合はヒットしていない
				if (Vector3::Dot(p.normal, Vector3::UnitY()) == 0.0f) { continue; }

				// 媒介変数tを求める
				float t = (distance - Vector3::Dot(pos, p.normal)) / Vector3::Dot(Vector3::UnitY(), p.normal);
				// 衝突点を求める
				Vector3 hitPosition = pos + (t * Vector3::UnitY());

				// 各辺を結んだベクトルと頂点と衝突点pを結んだベクトルのクロス積を取る
				Vector3 cross01 = Vector3::Cross((p.pos[0] - p.pos[1]), (p.pos[1] - hitPosition));
				Vector3 cross12 = Vector3::Cross((p.pos[1] - p.pos[2]), (p.pos[2] - hitPosition));
				Vector3 cross20 = Vector3::Cross((p.pos[2] - p.pos[0]), (p.pos[0] - hitPosition));

				// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
				if (Vector3::Dot(cross01, p.normal) >= 0.0f && Vector3::Dot(cross12, p.normal) >= 0.0f && Vector3::Dot(cross20, p.normal) >= 0.0f) {
#if DEMO
					triangleCube_[0].worldTF.translation = p.pos[0];
					triangleCube_[1].worldTF.translation = p.pos[1];
					triangleCube_[2].worldTF.translation = p.pos[2];
					triangleCube_[3].worldTF.translation = hitPosition;	// 4つ目はヒットしている場所
#endif
					// 衝突点がposより上だった場合 -> 座標を修正
					if (hitPosition.y > pos.y) {
						point->wtf->translation.y = hitPosition.y - point->offset.y;
						point->preFrameHit = true;
					}
					currentMorton = 0;	// モートンを0にして強制終了
					break;
				}
			}
			// 検証したモートン番号が0だった場合終了
			if (currentMorton == 0) { break; }
			// 検証が終わったので上の空間レベルへ
			currentMorton = (currentMorton - GetSpaceLevelObjectsSum(currentSpaceLevel--)) >> 3;
			currentMorton += GetSpaceLevelObjectsSum(currentSpaceLevel);
		};
	}
}

void Terrain::LoadModel(std::string filePath, const TransformQuat& wtf) {
	// モデル読み込み
	model_.LoadShortPath(filePath);
	model_.ApplyWorldTransform(wtf);

	// 必要なデータを用意
	const std::vector<uint32_t>& indexes = model_.GetModelData()->indexes; // インデックス
	StructuredBuffer<OutputVertexStruct>* vertices = model_.vertexBuffer_.get();	// 頂点

	// -- 8分木空間を生成 -- //

	// このモデルを囲む最小のAABB
	min_ = vertices->data_[0].position.xyz();
	max_ = min_;
	for (int i = 0; i < vertices->GetCount(); i++) {
		const Vector4& v = vertices->data_[i].position;
		min_.x = std::min<float>(v.x, min_.x);
		min_.y = std::min<float>(v.y, min_.y);
		min_.z = std::min<float>(v.z, min_.z);
		max_.x = std::max<float>(v.x, max_.x);
		max_.y = std::max<float>(v.y, max_.y);
		max_.z = std::max<float>(v.z, max_.z);
	}

	// このAABBを正立方体に
	Vector3 size = max_ - min_;	// 大きさ
	float maxSize = std::max<float>(std::max<float>(size.x, size.y), size.z);	// 最大の辺の長さを計算
	float maxSizeHalf = maxSize / 2.0f;	// 半分の長さ
	Vector3 center = (min_ + max_) / 2.0f;	// 中心点
	// 正立方体にする
	min_ = center - Vector3{ maxSizeHalf,maxSizeHalf,maxSizeHalf };
	max_ = center + Vector3{ maxSizeHalf,maxSizeHalf,maxSizeHalf };
	cellSize_ = maxSize / std::powf(2.0f, (float)kSubdivision_);	// 分割された空間の最小サイズを求める


	// -- モデルデータからポリゴンを生成 -- //
	for (int i = 0; i < indexes.size(); i += 3) {
		Terrain::Polygon p;
		p.pos[0] = vertices->data_[indexes[i]].position.xyz();
		p.pos[1] = vertices->data_[indexes[i + 1]].position.xyz();
		p.pos[2] = vertices->data_[indexes[i + 2]].position.xyz();
		p.normal = Vector3::Cross(p.pos[1] - p.pos[0], p.pos[2] - p.pos[1]).Normalize();	// 外積で面の法線を求める
		polygonMap_[GetMortonNumber(p)].push_back(p);	// 求めたモートン序列番号の元に格納
	}

#if DEMO
	// -- デバッグ用 -- //
	for (int i = 0; i < 4; i++) {
		triangleCube_[i].LoadCube();
		triangleCube_[i].worldTF.scale = { 0.1f,0.1f,0.1f };
		triangleCube_[i].SetAllMaterialLighting(false);
	}
	 
	// 空間の数
	int cubeCount = int(std::pow(2, kSubdivision_));
	// 立方体の幅
	Vector3 dist = (max_ - min_) / float(cubeCount);
	// 立方体のサイズ
	float cubeSize = maxSize / float(cubeCount) / 2.0f;

	for (int x = 0; x < cubeCount; x++) {
		for (int y = 0; y < cubeCount; y++) {
			for (int z = 0; z < cubeCount; z++) {
				cubes_.emplace_back();
				RigidModel& c = cubes_.back();
				c.LoadCube();
				c.worldTF.translation = min_ + (dist / 2.0f) + Vector3{ dist.x * float(x),dist.y * float(y),dist.z * float(z) };
				c.worldTF.scale = { cubeSize ,cubeSize ,cubeSize };
			}
		}
	}

	// こうしないとうまくワイヤーフレームにならない
	for (RigidModel& r : cubes_) {
		r.SetAllMaterialLighting(false);
		r.ChangeFillMode();
		r.isActive = false;
	}
#endif

	// 草を生成
	//System::engine->directXCommon_->GetRendererManager()->GenerateGrass(
	//	min_, max_,
	//	model_.GetModelData()->materials_[0].texture.t.GetIndex()
	//);

	
	//for (int i = 0; i < 50000; i++) {
	//	Vector3 pos;
	//	pos.x = min_.x + float(Utility::GenerateRandamNum<int>(0, 10000)) / 10000.0f * (max_.x - min_.x);
	//	pos.y = 0.0f;
	//	pos.z = min_.z + float(Utility::GenerateRandamNum<int>(0, 10000)) / 10000.0f * (max_.z - min_.z);

	//	// 地形上に出るように修正

	//	// 検索するモートン番号
	//	const uint32_t kTargetMorton = GetMortonNumber(pos);
	//	if (kTargetMorton == -1) { continue; }	// -1だった場合早期終了
	//	uint32_t currentMorton = kTargetMorton;	// 現在のモートン番号（下位レベルから検証）
	//	uint32_t currentSpaceLevel = kSubdivision_;	// 現在の空間レベル（下位レベルから検証）

	//	while (true) {
	//		// 現在の空間内の全オブジェクトと検証
	//		for (const Polygon& p : polygonMap_[currentMorton]) {
	//			// 平面のパラメータ
	//			float distance = p.normal.x * p.pos[0].x + p.normal.y * p.pos[0].y + p.normal.z * p.pos[0].z;
	//			// 垂直の場合はヒットしていない
	//			if (Vector3::Dot(p.normal, Vector3::UnitY()) == 0.0f) { continue; }

	//			// 媒介変数tを求める
	//			float t = (distance - Vector3::Dot(pos, p.normal)) / Vector3::Dot(Vector3::UnitY(), p.normal);
	//			// 衝突点を求める
	//			Vector3 hitPosition = pos + (t * Vector3::UnitY());

	//			// 各辺を結んだベクトルと頂点と衝突点pを結んだベクトルのクロス積を取る
	//			Vector3 cross01 = Vector3::Cross((p.pos[0] - p.pos[1]), (p.pos[1] - hitPosition));
	//			Vector3 cross12 = Vector3::Cross((p.pos[1] - p.pos[2]), (p.pos[2] - hitPosition));
	//			Vector3 cross20 = Vector3::Cross((p.pos[2] - p.pos[0]), (p.pos[0] - hitPosition));

	//			// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	//			if (Vector3::Dot(cross01, p.normal) >= 0.0f && Vector3::Dot(cross12, p.normal) >= 0.0f && Vector3::Dot(cross20, p.normal) >= 0.0f) {
	//				// 衝突点がposより上だった場合 -> 座標を修正
	//				if (hitPosition.y > pos.y) {
	//					pos.y = hitPosition.y;
	//				}
	//				currentMorton = 0;	// モートンを0にして強制終了
	//				break;
	//			}
	//		}
	//		// 検証したモートン番号が0だった場合終了
	//		if (currentMorton == 0) { break; }
	//		// 検証が終わったので上の空間レベルへ
	//		currentMorton = (currentMorton - GetSpaceLevelObjectsSum(currentSpaceLevel--)) >> 3;
	//		currentMorton += GetSpaceLevelObjectsSum(currentSpaceLevel);
	//	};

	//	// GPUで生成したいのでいったんコメントアウト
	//	//grassPositions_.Add(pos);
	//}
}

void Terrain::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::Text("Min : %f, %f, %f", min_.x, min_.y, min_.z);
	ImGui::Text("Max : %f, %f, %f", max_.x, max_.y, max_.z);
	ImGui::Text("CellSize : %f", cellSize_);
	ImGui::Checkbox("isActive", &isActive);
	// 当たり判定一覧
	if (!points_.list.empty()) {
		std::vector<const char*> itemText;
		for (int i = 0; i < points_.list.size(); i++) {
			itemText.push_back(std::to_string(i).c_str());
		}
		ImGui::ListBox("Collisions", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		Point* p = *Utility::GetIteratorAtIndex(points_.list, currentItem);
		p->wtf->DebugGUI();
		ImGui::DragFloat3("offset", &p->offset.x, 0.1f);
		int mortonNum = GetMortonNumber(p->offset + p->wtf->GetWorldPosition());	// 所属するモートン空間番号
		ImGui::Text("MortonNumber : %d", mortonNum);
	}
#if DEMO
	if (ImGui::Button("ChangeSpaceBorder")) {
		for (RigidModel& r : cubes_) {
			r.isActive = !r.isActive;
		}
	}
#endif
}

DWORD Terrain::BitSeparate(BYTE n) {
	n = (n | n << 8) & 0x0000f00f;
	n = (n | n << 4) & 0x000c30c3;
	n = (n | n << 2) & 0x00249249;
	return n;
}
DWORD Terrain::GetMortonOrder(const Math::Vector3& point) {
	return BitSeparate(BYTE(point.x)) | BitSeparate(BYTE(point.y)) << 1 | BitSeparate(BYTE(point.z)) << 2;
}
int Terrain::GetMortonNumber(Math::Vector3 point) {
	// AABBの外側の点ならば、-1を返す
	if(point.x < min_.x || point.y < min_.y || point.z < min_.z ||
		point.x > max_.x || point.y > max_.y || point.z > max_.z) {
		return -1;
	}

	// モートン番号を出すために座標を変換
	point -= min_;
	point /= cellSize_;

	// モートン番号
	int mortonNum = GetMortonOrder(point);
	// 線形8分木に直す
	mortonNum += GetSpaceLevelObjectsSum(kSubdivision_);	// 点は必ず一番小さい空間レベルになる

	// モートン空間番号を計算
	return mortonNum;
}
int Terrain::GetMortonNumber(const Terrain::Polygon& p) {
	// ポリゴンを囲むAABBを生成
	Vector3 min = p.pos[0];
	Vector3 max = min;
	for (int i = 0; i < 3; i++) {
		const Vector3& v = p.pos[i];
		min.x = std::min<float>(v.x, min.x);
		min.y = std::min<float>(v.y, min.y);
		min.z = std::min<float>(v.z, min.z);
		max.x = std::max<float>(v.x, max.x);
		max.y = std::max<float>(v.y, max.y);
		max.z = std::max<float>(v.z, max.z);
	}

	// モートン空間番号を出すために座標を変換
	min -= min_;
	min /= cellSize_;
	max -= min_;
	max /= cellSize_;

	// モートン空間番号を計算
	int minMortonNum = GetMortonOrder(min);
	int maxMortonNum = GetMortonOrder(max);

	// 排他的論理和を求める
	int xorMortonNum = minMortonNum ^ maxMortonNum;
	// 空間レベル
	int spaceLevel = kSubdivision_;

	// 空間レベルを出す
	while (xorMortonNum > 0) {
		xorMortonNum >>= 3;
		spaceLevel--;
	}

	// 領域の所属するモートン空間番号を出す
	int mortonNum = maxMortonNum >> ((kSubdivision_ - spaceLevel) * 3);
	// 線形8分木に直す
	mortonNum += GetSpaceLevelObjectsSum(spaceLevel);

	return mortonNum;
}

int Terrain::GetSpaceLevelObjectsSum(const int& spaceLevel) {
	if (spaceLevel < 0) { return 0; }	// 空間レベルが0未満の場合要素数は0
	return (int)((powf(8.0f, (float)(spaceLevel)) - 1.0f) / 7.0f);
}
