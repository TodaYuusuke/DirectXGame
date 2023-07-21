#pragma once
#include "../math/Math.h"

namespace LWP::Object {
	class WorldTransform {
	public: // パブリックな変数
		// ローカルスケール
		Math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
		// X,Y,Z軸回りのローカル回転角
		Math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };
		// ローカル座標
		Math::Vector3 translation = { 0.0f, 0.0f, 0.0f };

	public: // パブリックなメンバ関数
		WorldTransform() = default;
		WorldTransform(Math::Vector3 s, Math::Vector3 r, Math::Vector3 t) { 
			scale = s;
			rotation = r;
			translation = t;
		}
		~WorldTransform() = default;

		// ワールド座標
		Math::Vector3 GetWorldPosition() {
			Math::Matrix4x4 matWorld = GetMatWorld();
			return { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };
		};


		// ** プロパティ変数 ** //

	private: // 親となるワールド変換へのポインタ（読み取り専用）
		const WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		// 親関係を登録
		void SetParent(const WorldTransform* parent) { parent_ = parent; }

	private: // ワールド変換行列
		//Math::Matrix4x4 matWorld_{};
	public: // アクセッサ
		// ワールド行列を返す
		Math::Matrix4x4 GetMatWorld() const {
			Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation); 

			// 親があれば親のワールド行列を掛ける
			if (parent_) {
				result = result * parent_->GetMatWorld();
			}

			return result;
		}
	};
}