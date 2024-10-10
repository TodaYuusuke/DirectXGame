#include "QuadtreeSpaceDivision.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Object;
using namespace LWP::Math;

void QuadtreeSpaceDivision::Init() {
	divisionLevel = 4;
	spaceSize = 500.0f;
	centerPosition = { 0.0f,0.0f };
}

int QuadtreeSpaceDivision::GetMortonNumber(Vector3 position) {
	Vector2 p = { position.x, position.z };
	Vector2 min = GetMin();
	Vector2 max = GetMax();

	// 空間の外側の点ならば、-1を返す
	if (p.x < min.x || p.y < min.y ||  p.x > max.x || p.y > max.y) {
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
int QuadtreeSpaceDivision::GetMortonNumber(Vector3 min, Vector3 max) {
	// 計算に必要な変数
	Vector2 octMin = GetMin();
	//Vector2 octMax = GetMax();
	float cellSize = GetCellSize();

	// 空間の外側の点ならば、-1を返す
	/*if (p.x < min.x || p.y < min.y || p.z < min.z || p.x > max.x || p.y > max.y || p.z > max.z) {
		return -1;
	}*/

	// モートン空間番号を計算
	int minMortonNum = GetMortonOrder((Vector2{ min.x,min.z } - octMin) / cellSize);
	int maxMortonNum = GetMortonOrder((Vector2{ max.x,min.z } - octMin) / cellSize);

	// 排他的論理和を求める
	int xorMortonNum = minMortonNum ^ maxMortonNum;
	// 空間レベル
	int spaceLevel = divisionLevel - 1;

	// 空間レベルを出す
	while (xorMortonNum > 0) {
		xorMortonNum >>= 2;
		spaceLevel--;
	}

	// 領域の所属するモートン空間番号を出す
	int mortonNum = maxMortonNum >> ((divisionLevel - spaceLevel) * 2);
	// 線形8分木に直す
	mortonNum += GetSpaceLevelObjectsSum(spaceLevel);

	return mortonNum;
}

void QuadtreeSpaceDivision::DebugGUI() {
	// 分割レベル
	int d = static_cast<int>(divisionLevel);
	ImGui::InputInt("DivisionLevel", &d, 1);
	if (d < 1) { d = 1; }	// 1 以下にならないように
	divisionLevel = static_cast<uint32_t>(d);
	ImGui::DragFloat("SpaceSize", &spaceSize, 1.0f, 0.0f);	// 空間サイズ
	ImGui::DragFloat2("CenterPosition", &centerPosition.x, 0.01f);	// 空間中心点
}

DWORD QuadtreeSpaceDivision::BitSeparate(BYTE n) {
	n = (n | n << 8) & 0x00ff00ff;
	n = (n | n << 4) & 0x0f0f0f0f;
	n = (n | n << 2) & 0x33333333;
	return (n | n << 1) & 0x55555555;
}
DWORD QuadtreeSpaceDivision::GetMortonOrder(const Math::Vector2& point) {
	return BitSeparate(BYTE(point.x)) | (BitSeparate(BYTE(point.y)) << 1);
}
int QuadtreeSpaceDivision::GetSpaceLevelObjects(const int& spaceLevel) {
	return int(powf(4.0f, float(spaceLevel)));
}
int QuadtreeSpaceDivision::GetSpaceLevelObjectsSum(const int& spaceLevel) {
	if (spaceLevel < 0) { return 0; }	// 空間レベルが0未満の場合要素数は0
	return int((powf(4.0f, float(spaceLevel)) - 1.0f) / 3.0f);
}
