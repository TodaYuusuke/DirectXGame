#include "Terrain.h"

#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Resource;


Terrain::Terrain() {}

// 初期化
void Terrain::Initialize() {}
// 更新
void Terrain::Update(Base::RendererManager* manager) {
	manager;

	// 空間の最大分割数
	uint32_t maxResolution = 0;
	for (uint32_t i = 1; i <= kSubdivision_; i++) {
		maxResolution += (int)powf(8.0f, (float)i);
	}

	// 当たり判定を検証
	for (Point& p : points_) {
		// 座標を求める
		Vector3 pos = p.offset + p.wtf->GetWorldPosition();

		// 検索するモートン空間番号
		const uint32_t kMortonNum = GetMortonNumber(pos);
		uint32_t mortonNum = kMortonNum;
		mortonNum;
		/*
		while (true) {
			// 現在の空間内に登録されているオブジェクトと当たり判定検証
			if (!polygonMap_[mortonNum].empty()) {
				for (const Polygon& polygon : polygonMap_[mortonNum]) {
					// ここで当たり判定検証
					polygon;
				}
			}

			// 次の小空間が8分木分割数を超えていなければ移動
			if ((mortonNum << 3) + 1 < maxResolution) {
				mortonNum = (mortonNum << 3) + 1;
				// 最初に戻る
				continue;
			}
			// そうでない場合は次のモートン番号に移動する
			else if (mortonNum % 4 != 0) {
				mortonNum++;
				// 最初に戻る
				continue;
			}

			// 上の空間に所属する小空間をすべて検証し終わった場合

			// １つ上の空間に戻る
			do {
				mortonNum = (mortonNum - 1) >> 3;
				// 戻った空間がその空間の最後の数値の場合 -> もう一度戻る
			} while (mortonNum % 4 == 0);

			// 次のモートン番号へ進む
			mortonNum++;

			// 戻った空間が最初の空間（kMortonNum）だった場合 -> ループ終了
			if (mortonNum == kMortonNum) {
				break;
			}
		}
		*/
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
		p.normal = Vector3::Cross(p.pos[2] - p.pos[0], p.pos[1] - p.pos[0]).Normalize();	// 外積で面の法線を求める
		polygonMap_[GetMortonNumber(p)].push_back(p);	// 求めたモートン序列番号の元に格納
	}

#if DEMO
	// -- デバッグ用 -- //
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

	// なぜかこうしないとうまくワイヤーフレームにならない
	for (RigidModel& r : cubes_) {
		r.SetAllMaterialLighting(false);
		r.ChangeFillMode();
	}
#endif
}
void Terrain::SetNewCollider(Math::Vector3 offset, TransformQuat* wtf) {
	points_.push_back({ offset,wtf });
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
	if (!points_.empty()) {
		std::vector<const char*> itemText;
		for (int i = 0; i < points_.size(); i++) {
			itemText.push_back(std::to_string(i).c_str());
		}
		ImGui::ListBox("Collisions", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		Point& p = points_[currentItem];
		p.wtf->DebugGUI();
		ImGui::DragFloat3("offset", &p.offset.x, 0.1f);
		int mortonNum = GetMortonNumber(p.offset + p.wtf->GetWorldPosition());	// 所属するモートン空間番号
		ImGui::Text("MortonNumber : %d", mortonNum);
	}
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
	mortonNum += (int)((powf(8.0f, (float)(kSubdivision_)) - 1.0f) / 7.0f);	// 点は必ず一番小さい空間レベルになる

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
	//min -= min_;
	min /= cellSize_;
	//max -= AABBmin_;
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
	mortonNum += (int)((powf(8.0f, (float)(spaceLevel)) - 1.0f) / 7.0f);

	return mortonNum;
}