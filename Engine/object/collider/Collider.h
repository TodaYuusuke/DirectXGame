#pragma once
#include "shape/cAABB.h"
#include "shape/cSphere.h"

#include "Mask.h"

#include "utility/Index.h"

#include <variant>
#include <functional>
#include <map>

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のクラス
	/// </summary>
	class Collider {
	public: // ** パブリックなメンバ変数 ** //
		// トランスフォーム
		Object::TransformQuat worldTF;
		// 固有名詞
		std::string name = "Collider";
		// マスク
		Mask mask;

		// 動くかのフラグ
		bool isMove = true;
		// アクティブ切り替え
		bool isActive = true;
		
		// Variant
		using ShapeVariant = std::variant<AABB, Sphere>;

		// ブロードフェーズのコライダー形状
		ShapeVariant broad;
		// ナローフェーズのコライダー形状
		std::vector<ShapeVariant> narrows;


		// - ヒット時のリアクション用の関数 - //
		typedef std::function<void(Collider* hitTarget)> OnHitFunction;	// ヒット時の関数ポインタの型

		// フレーム中になにともヒットしていないとき
		//OnHitFunction noHitLambda = [](Collider* hitTarget) { hitTarget; };
		// ヒットした瞬間のとき
		OnHitFunction enterLambda = [](Collider* hitTarget) { hitTarget; };
		// ヒットし続けているとき
		OnHitFunction stayLambda = [](Collider* hitTarget) { hitTarget; };
		// ヒットが離れたとき
		OnHitFunction exitLambda = [](Collider* hitTarget) { hitTarget; };


	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Collider();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Collider();

		// 更新処理
		void Update();

		// 追従するトランスフォームをペアレントにセットする関数
		void SetFollowTarget(Object::TransformQuat* ptr) { worldTF.Parent(ptr); }
		// ヒット時に正常な位置に修正するベクトルを加算
		//void AdjustPosition(const LWP::Math::Vector3& fixVector) { followModel_.t->worldTF += fixVector; }

		// 渡された形との当たり判定を確認する関数
		void CheckCollision(Collider* c);
		// ヒット時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void Hit(Collider* c);
		// ヒットしていなかった時の処理（受け身のとき相手に呼び出してもらうためにpublic）
		void NoHit(Collider* c);
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
			GetBasePtr(broad)->SetFollowPtr(&worldTF);	// followをセット
			return std::get<T>(broad);
		}

	private: // ** メンバ変数 ** //

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
		ICollisionShape* GetBasePtr(ShapeVariant& variant);
		
		/// <summary>
		/// 現在の型名を取得する関数
		/// </summary>
		/// <param name="variant">共通化された型</param>
		/// <returns>現在入っているクラス名</returns>
		std::string GetCurrentTypeName(const ShapeVariant& variant);

		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ブロードフェーズ）
		/// </summary>
		bool CheckBroadCollision(ShapeVariant& c);
		/// <summary>
		/// 共通化変数同士の当たり判定チェックの関数（ナローフェーズ
		/// </summary>
		bool CheckNarrowsCollision(Collider* c);
	};
};
