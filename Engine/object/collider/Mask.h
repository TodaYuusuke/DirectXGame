#pragma once
#include <cstdint>

// 当たり判定マスク用のビット値
#define ColMask0 0b1		// 1
#define ColMask1 0b1 << 1	// 10
#define ColMask2 0b1 << 2	// 100
#define ColMask3 0b1 << 3	// 1000
#define ColMask4 0b1 << 4	// 10000
#define ColMask5 0b1 << 5	// 100000
#define ColMask6 0b1 << 6	// 1000000
#define ColMask7 0b1 << 7	// 10000000
#define ColMask8 0b1 << 8	// 100000000
#define ColMask9 0b1 << 9	// 1000000000
#define ColMask10 0b1 << 10	// 10000000000
#define ColMask11 0b1 << 11	// 100000000000
#define ColMask12 0b1 << 12	// 1000000000000
#define ColMask13 0b1 << 13	// 10000000000000
#define ColMask14 0b1 << 14	// 100000000000000
#define ColMask15 0b1 << 15	// 1000000000000000
#define ColMaskALL (0b1 << 16) | !(0b1 << 16)	// 1111111111111111

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定のマスク処理をクラス化
	/// </summary>
	class Mask {
	public: // ** パブリックなメンバ関数 ** //
		// 引数のマスクが所属しているグループと重なるか検証する関数
		bool CheckBelong(Mask hit) { return belongFrag & hit.GetHitFrag(); }

	private: // ** プロパティ変数 ** //
		// 衝突を検証するグループのフラグ
		uint32_t hitFrag = ColMaskALL;
	public:
		// ゲッター
		uint32_t GetHitFrag() { return hitFrag; }
		// セッター
		void SetHitFrag(uint32_t frag) { hitFrag = frag; }


	private: // ** プロパティ変数 ** //
		// 自身が属するグループのフラグ
		uint32_t belongFrag = ColMaskALL;
	public:
		// ゲッター
		uint32_t GetBelongFrag() { return belongFrag; }
		// セッター
		void SetBelongFrag(uint32_t frag) { belongFrag = frag; }

	};
};