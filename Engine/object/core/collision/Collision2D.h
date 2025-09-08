#pragma once
#include "collider/child2D/cCircle.h"
#include "collider/child2D/cRectangle.h"

#include "utility/Index.h"

#include "QuadtreeSpaceDivision.h"

#include <string>
#include <variant>
#include <functional>
#include <map>
#include "CollisionMask.h"

namespace LWP::Object {
	/// <summary>
	/// 2Dの当たり判定用のクラス
	/// </summary>
	class Collision2D {

	public: // ** パブリックなメンバ変数 ** //

		// 固有名詞
		std::string name = "Collider2D";

		// トランスフォーム
		Object::TransformQuat worldTF;
		// マスク
		CollisionMask mask;

		// 動くかのフラグ
		bool isMove = false;
		// アクティブ切り替え
		bool isActive = true;
		
		// Variant
		using ShapeVariant2D = std::variant<Collider2D::Circle, Collider2D::Rectangle>;
		// ブロードフェーズのコライダー形状
		ShapeVariant2D broad;
		// ナローフェーズのコライダー形状
		std::vector<ShapeVariant2D> narrows;

		// - ヒット時のリアクション用の関数 - //
		typedef std::function<void(Collision2D* hitTarget)> OnHitFunction;	// ヒット時の関数ポインタの型

		// フレーム中になにともヒットしていないとき
		//OnHitFunction noHitLambda = [](ICollider* hitTarget) { hitTarget; };
		// ヒットした瞬間のとき
		OnHitFunction enterLambda = [](Collision2D* hitTarget) { hitTarget; };
		// ヒットし続けているとき
		OnHitFunction stayLambda = [](Collision2D* hitTarget) { hitTarget; };
		// ヒットが離れたとき
		OnHitFunction exitLambda = [](Collision2D* hitTarget) { hitTarget; };


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Collision2D();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Collision2D();

		// 更新処理
		void Update();

		// 追従を解除
		void UnSetFollow();
		// 追従する対象をセット（トランスフォーム）
		void SetFollow(Object::TransformQuat* ptr);
		// オクトツリーをセットする関数
		void SetQuadTree(Object::QuadtreeSpaceDivision* quadTree) { quadTree_ = quadTree; }
		// シリアル番号をセットする関数
		int GetMortonNumber() { return mortonNumber; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void ApplyFixVector(const Math::Vector3& fixVector);

		// マスクチェック
		bool CheckMask(Collision2D* c);
		// 渡された形との当たり判定を確認する関数
		void CheckCollision(Collision2D* c);
		// ヒット時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void Hit(Collision2D* c);
		// ヒットしていなかった時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void NoHit(Collision2D* c);
		// シリアル番号をセットする関数
		void SetSerial(Utility::Index&& s) { serialNum = std::move(s); }
		// シリアル番号を返す関数
		int GetSerial() { return serialNum; }

		// スクリーン座標を取得
		LWP::Math::Vector2 GetScreenPosition();
		// ImGui
		void DebugGUI();


		/// <summary>
		/// ブロードフェーズの形状をセットする関数
		/// </summary>
		/// <typeparam name="T">IColliderShapeを継承したクラスのみ</typeparam>
		/// <param name="t">代入する実態</param>
		/// <returns>代入された実体への参照を返す</returns>
		template<IsICollider2D T>
		T& SetBroadShape() {
			broad.emplace<T>();
			GetBasePtr(broad)->SetFollowPtr(&worldTF);
			return std::get<T>(broad);
		}

	private: // ** メンバ変数 ** //

		// オクトツリーのポインタ
		Object::QuadtreeSpaceDivision* quadTree_;

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
		Collider2D::ICollider* GetBasePtr(ShapeVariant2D& variant);
		
		/// <summary>
		/// 現在の型名を取得する関数
		/// </summary>
		/// <param name="variant">共通化された型</param>
		/// <returns>現在入っているクラス名</returns>
		std::string GetCurrentTypeName(const ShapeVariant2D& variant);

		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ブロードフェーズ）
		/// </summary>
		bool CheckBroadCollision(ShapeVariant2D& c, Math::Vector3* fixVec);
		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ナローフェーズ
		/// </summary>
		bool CheckNarrowsCollision(Collision2D* c, Math::Vector3* fixVec);
	};
};
