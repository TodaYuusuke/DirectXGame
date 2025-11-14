#pragma once
#include <cstdint>

// 当たり判定マスク用のビット値
#define ColMaskNone 0b0		// 0000000000000000
#define ColMask0 0b1		// 0000000000000001
#define ColMask1 0b1 << 1	// 0000000000000010
#define ColMask2 0b1 << 2	// 0000000000000100
#define ColMask3 0b1 << 3	// 0000000000001000
#define ColMask4 0b1 << 4	// 0000000000010000
#define ColMask5 0b1 << 5	// 0000000000100000
#define ColMask6 0b1 << 6	// 0000000001000000
#define ColMask7 0b1 << 7	// 0000000010000000
#define ColMask8 0b1 << 8	// 0000000100000000
#define ColMask9 0b1 << 9	// 0000001000000000
#define ColMask10 0b1 << 10	// 0000010000000000
#define ColMask11 0b1 << 11	// 0000100000000000
#define ColMask12 0b1 << 12	// 0001000000000000
#define ColMask13 0b1 << 13	// 0010000000000000
#define ColMask14 0b1 << 14	// 0100000000000000
#define ColMask15 0b1 << 15	// 1000000000000000
#define ColMaskALL (0b1 << 16) - 0b1	// 1111111111111111

namespace LWP::Object {
	/// <summary>
	/// 当たり判定のマスク処理用クラス
	/// </summary>
	class CollisionMask {
	public: // ** パブリックなメンバ関数 ** //
		// 引数のマスクが所属しているグループと重なるか検証する関数/
		bool CheckBelong(CollisionMask hit) { return belongFrag & hit.GetHitFrag(); }
		// デバッグ用ImGui
		void DebugGUI();

	private: // ** プロパティ変数 ** //
		// 衝突を検証するグループのフラグ
		uint32_t hitFrag = ColMaskALL;
		// 自身が属するグループのフラグ
		uint32_t belongFrag = ColMaskALL;

	public: // ** アクセッサ ** //
		// ゲッター
		uint32_t GetHitFrag() { return hitFrag; }
		uint32_t GetBelongFrag() { return belongFrag; }
		// セッター
		void SetHitFrag(uint32_t frag) { hitFrag = frag; }
		void SetBelongFrag(uint32_t frag) { belongFrag = frag; }
	};
};
