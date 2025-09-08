#pragma once
#include "math/vector/Vector2.h"
#include "math/Rectangle.h"
#include <map>
#include <list>
#include <cmath>
#include <windows.h>

namespace LWP::Object {
	class Collision2D;	// 前方宣言

	/// <summary>
	/// 4分木空間分割クラス
	/// </summary>
	class QuadtreeSpaceDivision final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		QuadtreeSpaceDivision() { Init(); }
		/// <summary>
		/// デストラクタ
		/// </summary>
		~QuadtreeSpaceDivision() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// リストに含まれる当たり判定をチェック
		/// </summary>
		/// <param name="list"></param>
		void CheckAllCollisions(std::list<Object::Collision2D*>* list);

		/// <summary>
		/// モートン序列番号を取得
		/// </summary>
		/// <param name="position">... 取得する座標</param>
		/// <returns>モートン序列番号</returns>
		int GetMortonNumber(Math::Vector2 position);
		/// <summary>
		/// モートン序列番号を取得
		/// </summary>
		/// <param name="min">... AABBのmin座標</param>
		/// <param name="max">... AABBのmax座標</param>
		/// <returns>モートン序列番号</returns>
		int GetMortonNumber(Math::Rectangle2D rect);

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
		uint32_t divisionLevel = 0;
		// 空間のサイズ
		float spaceSize = 2000.0f;
		// 空間の中心点
		LWP::Math::Vector2 centerPosition = { 0.0f,0.0f };
		

	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 空間の最小地点を取得する関数
		/// </summary>
		/// <returns>空間のmin座標</returns>
		Math::Vector2 GetMin() { return centerPosition - Math::Vector2{ spaceSize,spaceSize }; }
		/// <summary>
		/// 空間の最大地点を取得する関数
		/// </summary>
		/// <returns>空間のmax座標</returns>
		Math::Vector2 GetMax() { return centerPosition + Math::Vector2{ spaceSize,spaceSize }; }
		/// <summary>
		/// 最小の空間サイズを取得する関数
		/// </summary>
		/// <returns>空間のmax座標</returns>
		float GetCellSize() { return (spaceSize * 2.0f) / std::powf(2.0f, (float)divisionLevel); }
		
		/// <summary>
		/// ビットを2ビットごとに広げる関数
		/// </summary>
		DWORD BitSeparate(BYTE n);
		/// <summary>
		/// 点のモートン序列番号を求める関数
		/// </summary>
		DWORD GetMortonOrder(const Math::Vector2& point);

	};
}
