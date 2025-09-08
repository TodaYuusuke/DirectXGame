#include "OctreeSpaceDivision.h"
#include "Collision.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Object;
using namespace LWP::Math;

void OctreeSpaceDivision::Init() {
	divisionLevel = 4;
	spaceSize = 500.0f;
	centerPosition = { 0.0f,0.0f,0.0f };
}
void OctreeSpaceDivision::CheckAllCollisions(std::list<Object::Collision*>* list) {
	// モートン順に並べたmapを作る
	std::map<int, std::vector<Collision*>> map;
	std::list<Collision*> stackList;	// 上位空間のコリジョンをまとめたデータ
	int maxResolution = GetSpaceLevelObjectsSum(divisionLevel + 1);	// 空間の最大分割数

	// 全体を更新
	for (Collision* c : *list) {
		c->Update();
		map[c->GetMortonNumber()].push_back(c);	// mapに配置
	}

	int currentNum = 0;			// 現在検証中のモートン序列番号

	while (true) {
		// 現在の空間に存在するコリジョンの数
		int size = static_cast<int>(map[currentNum].size());
		// 現在の空間内に登録されているオブジェクト同士を衝突
		for (int i = 0; i < size; i++) {
			for (int j = i + 1; j < size; j++) {
				map[currentNum][i]->CheckCollision(map[currentNum][j]);	// 今のコリジョン × 次のコリジョン
			}

			// スタック（リスト）に登録されているコリジョンとも衝突判定
			for (std::list<Collision*>::iterator it = stackList.begin(); it != stackList.end(); it++) {
				map[currentNum][i]->CheckCollision(*it);
			}
		}

		// -- 次の空間検索開始 -- //

		// 次の小空間が8分木分割数を超えていなければ移動
		if ((currentNum << 3) + 1 < maxResolution) {
			// スタックリストにこの空間のデータを追加
			for (int i = 0; i < size; i++) {
				stackList.push_back(map[currentNum][i]);
			}
			currentNum = (currentNum << 3) + 1;
			// 最初に戻る
			continue;
		}
		// そうでない場合は次のモートン番号に移動する
		else if (currentNum % 8 != 0) {
			currentNum++;
			// 最初に戻る
			continue;
		}

		// 下の空間に所属する小空間をすべて検証し終わった場合

		// １つ上の空間に戻る
		do {
			currentNum = (currentNum - 1) >> 3;
			// currentNumが実数値の場合 -> スタックからオブジェクトを外す
			if (currentNum >= 0) {
				size_t PopNum = map[currentNum].size();
				for (int i = 0; i < PopNum; i++) {
					stackList.pop_back();
				}
			}
			// 戻った空間がその空間の最後の数値の場合 -> もう一度戻る
		} while (currentNum % 8 == 0);

		// 次のモートン番号へ進む
		currentNum++;

		// 戻った空間が場外（-1）だった場合 -> ループ終了
		if (currentNum == 0) {
			break;
		}
	}

	// 全て終わった後にmapとlistをクリア
	map.clear();
	stackList.clear();
}

int OctreeSpaceDivision::GetMortonNumber(Vector3 position) {
	Vector3 p = position;
	Vector3 min = GetMin();
	Vector3 max = GetMax();

	// 空間の外側の点ならば、-1を返す
	if (p.x < min.x || p.y < min.y || p.z < min.z  ||  p.x > max.x || p.y > max.y || p.z > max.z) {
		return -1;
	}

	// モートン番号を出すために座標を変換
	p -= min;
	p /= GetCellSize();

	// モートン番号
	int mortonNum = GetMortonOrder(p);
	// 線形8分木に直す
	mortonNum += GetSpaceLevelObjectsSum(divisionLevel);	// 点は必ず一番小さい空間レベルになる

	// モートン空間番号を計算
	return mortonNum;
}
int OctreeSpaceDivision::GetMortonNumber(Vector3 min, Vector3 max) {
	// 計算に必要な変数
	Vector3 octMin = GetMin();
	//Vector3 octMax = GetMax();
	float cellSize = GetCellSize();

	// 空間の外側の点ならば、-1を返す
	/*if (p.x < min.x || p.y < min.y || p.z < min.z || p.x > max.x || p.y > max.y || p.z > max.z) {
		return -1;
	}*/

	// モートン空間番号を計算
	int minMortonNum = GetMortonOrder((min - octMin) / cellSize);
	int maxMortonNum = GetMortonOrder((max - octMin) / cellSize);

	// 排他的論理和を求める
	int xorMortonNum = minMortonNum ^ maxMortonNum;
	// 空間レベル
	int spaceLevel = divisionLevel - 1;

	// 空間レベルを出す
	while (xorMortonNum > 0) {
		xorMortonNum >>= 3;
		spaceLevel--;
	}

	// 領域の所属するモートン空間番号を出す
	int mortonNum = maxMortonNum >> ((divisionLevel - spaceLevel) * 3);
	// 線形8分木に直す
	mortonNum += GetSpaceLevelObjectsSum(spaceLevel);

	return mortonNum;
}

void OctreeSpaceDivision::DebugGUI() {
	// 分割レベル
	int d = static_cast<int>(divisionLevel);
	ImGui::InputInt("DivisionLevel", &d, 1);
	if (d < 1) { d = 1; }	// 1 以下にならないように
	divisionLevel = static_cast<uint32_t>(d);
	// 空間サイズ
	ImGui::DragFloat("SpaceSize", &spaceSize, 1.0f, 0.0f);
	// 空間中心点
	ImGui::DragFloat3("CenterPosition", &centerPosition.x, 0.01f);
}

DWORD OctreeSpaceDivision::BitSeparate(BYTE n) {
	n = (n | n << 8) & 0x0000f00f;
	n = (n | n << 4) & 0x000c30c3;
	n = (n | n << 2) & 0x00249249;
	return n;
}
DWORD OctreeSpaceDivision::GetMortonOrder(const Math::Vector3& point) {
	return BitSeparate(BYTE(point.x)) | BitSeparate(BYTE(point.y)) << 1 | BitSeparate(BYTE(point.z)) << 2;
}
int OctreeSpaceDivision::GetSpaceLevelObjects(const int& spaceLevel) {
	return int(powf(8.0f, float(spaceLevel)));
}
int OctreeSpaceDivision::GetSpaceLevelObjectsSum(const int& spaceLevel) {
	if (spaceLevel < 0) { return 0; }	// 空間レベルが0未満の場合要素数は0
	return int((powf(8.0f, float(spaceLevel)) - 1.0f) / 7.0f);
}
