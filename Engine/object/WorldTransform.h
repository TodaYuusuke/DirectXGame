#pragma once
#include "../math/Math.h"

namespace LWP::Object {
	class WorldTransform {
	public: // パブリックな変数
		// ローカルスケール
		Math::Vector3 scale;
		// X,Y,Z軸回りのローカル回転角
		Math::Vector3 rotation;
		// ローカル座標
		Math::Vector3 translation;

	public: // パブリックなメンバ関数
		WorldTransform();
		WorldTransform(Math::Vector3 s, Math::Vector3 r, Math::Vector3 t);
		WorldTransform(Math::Vector3 r, Math::Vector3 t);
		WorldTransform(Math::Vector3 t);
		~WorldTransform() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// ワールド座標を返す
		/// </summary>
		Math::Vector3 GetWorldPosition();


		// ** プロパティ変数 ** //

	private: // 親となるワールド変換へのポインタ（読み取り専用）
		const WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		// 親関係を登録
		void SetParent(const WorldTransform* parent);

	private: // ワールド変換行列
		//Math::Matrix4x4 matWorld_{};
	public: // アクセッサ
		// ワールド行列を返す
		Math::Matrix4x4 GetMatWorld() const;

	};
}