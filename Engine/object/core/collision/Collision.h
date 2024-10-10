#pragma once
#include "collider/child/cPoint.h"
#include "collider/child/cAABB.h"
#include "collider/child/cSphere.h"
#include "collider/child/cCapsule.h"
#include "collider/child/cMesh.h"
#include "collider/child/cTerrain.h"

#include "OctreeSpaceDivision.h"

#include "utility/Index.h"

#include <variant>
#include <functional>
#include <map>

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
	/// 当たり判定用のクラス
	/// </summary>
	class Collision {
	public: // ** 内包クラス ** //
		class Mask {
		public: // ** パブリックなメンバ関数 ** //
			// 引数のマスクが所属しているグループと重なるか検証する関数/
			bool CheckBelong(Mask hit) { return belongFrag & hit.GetHitFrag(); }
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

	public: // ** パブリックなメンバ変数 ** //
		// トランスフォーム
		Object::TransformQuat worldTF;

		// 固有名詞
		std::string name = "Collider";
		// マスク
		Mask mask;

		// 動くかのフラグ
		bool isMove = false;
		// アクティブ切り替え
		bool isActive = true;
		
		// Variant
		using ShapeVariant = std::variant<Collider::Point, Collider::AABB, Collider::Sphere, Collider::Capsule, Collider::Mesh, Collider::Terrain>;

		// ブロードフェーズのコライダー形状
		ShapeVariant broad;
		// ナローフェーズのコライダー形状
		std::vector<ShapeVariant> narrows;


		// - ヒット時のリアクション用の関数 - //
		typedef std::function<void(Collision* hitTarget)> OnHitFunction;	// ヒット時の関数ポインタの型

		// フレーム中になにともヒットしていないとき
		//OnHitFunction noHitLambda = [](Collider* hitTarget) { hitTarget; };
		// ヒットした瞬間のとき
		OnHitFunction enterLambda = [](Collision* hitTarget) { hitTarget; };
		// ヒットし続けているとき
		OnHitFunction stayLambda = [](Collision* hitTarget) { hitTarget; };
		// ヒットが離れたとき
		OnHitFunction exitLambda = [](Collision* hitTarget) { hitTarget; };


	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Collision();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Collision();

		// 更新処理
		void Update();

		// 追従するトランスフォームをペアレントにセットする関数
		void SetFollowTarget(Object::TransformQuat* ptr);
		// オクトツリーをセットする関数
		void SetOctree(Object::OctreeSpaceDivision* octree) { octree_ = octree; }
		// シリアル番号をセットする関数
		int GetMortonNumber() { return mortonNumber; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void ApplyFixVector(const LWP::Math::Vector3& fixVector);

		// マスクチェック
		bool CheckMask(Collision* c);
		// 渡された形との当たり判定を確認する関数
		void CheckCollision(Collision* c);
		// ヒット時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void Hit(Collision* c);
		// ヒットしていなかった時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void NoHit(Collision* c);
		// シリアル番号をセットする関数
		void SetSerial(Utility::Index&& s) { serialNum = std::move(s); }
		// シリアル番号を返す関数
		int GetSerial() { return serialNum; }

		// ワールド座標を取得
		LWP::Math::Vector3 GetWorldPosition() { return worldTF.GetWorldPosition(); }
		// ImGui
		void DebugGUI();


		/// <summary>
		/// ブロードフェーズの形状をセットする関数
		/// </summary>
		/// <typeparam name="T">IColliderShapeを継承したクラスのみ</typeparam>
		/// <param name="t">代入する実態</param>
		/// <returns>代入された実体への参照を返す</returns>
		template<IsICollider T>
		T& SetBroadShape(const T& t) {
			broad = t;
			GetBasePtr(broad)->SetFollowPtr(&worldTF);
			return std::get<T>(broad);
		}

	private: // ** メンバ変数 ** //

		// 追従するトランスフォーム
		Object::TransformQuat* followTF;
		// オクトツリーのポインタ
		Object::OctreeSpaceDivision* octree_;

		// モートン序列番号
		int mortonNumber = -1;
		// 自身のシリアル番号
		Utility::Index serialNum;
		// 多重ヒット回避用のヒット対象を保持する変数<シリアル番号、ヒット回数（呼び出す関数を識別)>
		std::map<int, int> serialMap;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 派生クラスから基底クラスのポインタに変換する関数
		/// </summary>
		/// <param name="variant">共通化された型</param>
		/// <returns>基底クラス型のポインタ</returns>
		Collider::ICollider* GetBasePtr(ShapeVariant& variant);
		
		/// <summary>
		/// 現在の型名を取得する関数
		/// </summary>
		/// <param name="variant">共通化された型</param>
		/// <returns>現在入っているクラス名</returns>
		std::string GetCurrentTypeName(const ShapeVariant& variant);

		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ブロードフェーズ）
		/// </summary>
		bool CheckBroadCollision(ShapeVariant& c, Math::Vector3* fixVec);
		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ナローフェーズ
		/// </summary>
		bool CheckNarrowsCollision(Collision* c, Math::Vector3* fixVec);
	};
};
