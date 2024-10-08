#include "OctreeSpaceDivision.h"

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
	int spaceLevel = divisionLevel;

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
