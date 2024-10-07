#pragma once
#include "math/vector/Vector3.h"
#include <map>
#include <list>
#include <cmath>

#if DEMO
#include "primitive/3d/Cube.h"
#endif



namespace LWP::Object {
	/// <summary>
	/// 8分木空間分割クラス
	/// </summary>
	class OctreeSpaceDivision final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		OctreeSpaceDivision() { Init(); }
		/// <summary>
		/// デストラクタ
		/// </summary>
		~OctreeSpaceDivision() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// モートン序列番号を取得
		/// </summary>
		/// <param name="position">... 取得する座標</param>
		/// <returns>モートン序列番号</returns>
		int GetMortonNumber(LWP::Math::Vector3 position);
		/// <summary>
		/// モートン序列番号を取得
		/// </summary>
		/// <param name="min">... AABBのmin座標</param>
		/// <param name="max">... AABBのmax座標</param>
		/// <returns>モートン序列番号</returns>
		int GetMortonNumber(LWP::Math::Vector3 min, LWP::Math::Vector3 max);

		/// <summary>
		/// その空間レベルの要素数を返す関数
		/// </summary>
		int GetSpaceLevelObjects(const int& spaceLevel);
		/// <summary>
		/// その空間レベルまで（この空間レベルは含まない）の要素数合計を返す関数
		/// </summary>
		int GetSpaceLevelObjectsSum(const int& spaceLevel);
		
		/// <summary>
		/// デバッグ用GUI
		/// </summary>
		void DebugGUI();
		

	public: // ** パブリックなメンバ変数 ** //

		// 空間分割レベル
		uint32_t divisionLevel = 4;
		// 空間のサイズ
		float spaceSize = 1000.0f;
		// 空間の中心点
		LWP::Math::Vector3 centerPosition = { 0.0f,0.0f,0.0f };
		

	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 空間の最小地点を取得する関数
		/// </summary>
		/// <returns>空間のmin座標</returns>
		LWP::Math::Vector3 GetMin() { return centerPosition - LWP::Math::Vector3{ spaceSize,spaceSize,spaceSize }; }
		/// <summary>
		/// 空間の最大地点を取得する関数
		/// </summary>
		/// <returns>空間のmax座標</returns>
		LWP::Math::Vector3 GetMax() { return centerPosition + LWP::Math::Vector3{ spaceSize,spaceSize,spaceSize }; }
		/// <summary>
		/// 最小の空間サイズを取得する関数
		/// </summary>
		/// <returns>空間のmax座標</returns>
		float GetCellSize() { return (spaceSize * 2.0f) / std::powf(2.0f, (float)divisionLevel); }
		
		/// <summary>
		/// ビットを3ビットごとに広げる関数
		/// </summary>
		DWORD BitSeparate(BYTE n);
		/// <summary>
		/// 点のモートン序列番号を求める関数
		/// </summary>
		DWORD GetMortonOrder(const Math::Vector3& point);

	};
}
