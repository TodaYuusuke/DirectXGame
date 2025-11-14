#pragma once
#include "collider/child/cPoint.h"
#include "collider/child/cAABB.h"
#include "collider/child/cSphere.h"
#include "collider/child/cCapsule.h"

#include "OctreeSpaceDivision.h"

#include "utility/other/Index.h"

#include <variant>
#include <functional>
#include <map>
#include "CollisionMask.h"


namespace LWP::Object {
	/// <summary>
	/// 当たり判定用のクラス
	/// </summary>
	class Collision {
	public: // ** パブリックなメンバ変数 ** //

		// 固有名詞
		std::string name = "Collider";

		// トランスフォーム
		TransformQuat worldTF;
		// マスク
		CollisionMask mask;

		// 動くかのフラグ
		bool isMove = false;
		// アクティブ切り替え
		bool isActive = true;
		
		// Variant
		using ShapeVariant = std::variant<Collider::Point, Collider::AABB, Collider::Sphere, Collider::Capsule>;
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

		// 追従を解除
		void UnSetFollow();
		// 追従する対象をセット（トランスフォーム）
		void SetFollow(Object::TransformQuat* ptr);
		// 追従する対象をセット（スキンモデルのJoint）
		void SetFollow(Resource::SkinningModel* model, const std::string& jointName);
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
		/// <returns>代入された実体への参照を返す</returns>
		template<IsICollider T>
		T& SetBroadShape() {
			broad.emplace<T>();
			GetBasePtr(broad)->SetFollowPtr(&worldTF);
			return std::get<T>(broad);
		}

	private: // ** メンバ変数 ** //

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
