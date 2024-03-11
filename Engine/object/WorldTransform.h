#pragma once
#include "../math/Math.h"
#include <string>
#include "utility/observers/Observer.h"

namespace LWP::Object {
	class WorldTransform {
	public: // ** パブリックな変数 ** //
		// ローカル座標
		Math::Vector3 translation;
		// X,Y,Z軸回りのローカル回転角
		Math::Vector3 rotation;
		// ローカルスケール
		Math::Vector3 scale;


	private: // ** プロパティ変数 ** //
		// 親となるワールド変換へのポインタ（読み取り専用）
		Utility::Observer<WorldTransform*>* parent_ = nullptr;
		//WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		// 親関係を登録
		void Parent(WorldTransform* parent);
		// 親のポインタを受け取る（実装禁止）
		//WorldTransform* Parent();
		

	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		WorldTransform();
		WorldTransform(Math::Vector3 t, Math::Vector3 r, Math::Vector3 s);
		WorldTransform(Math::Vector3 t, Math::Vector3 r);
		WorldTransform(Math::Vector3 t);
		~WorldTransform() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// ワールド行列を返す
		/// </summary>
		Math::Matrix4x4 GetWorldMatrix() const;
		/// <summary>
		/// ワールド座標を返す
		/// </summary>
		Math::Vector3 GetWorldPosition() const;


		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "WorldTransform");
		/// <summary>
		/// 2D用のWorldTransform
		/// </summary>
		void DebugGUI2D(const std::string& label = "WorldTransform");

	public: // ** オペレーターオーバーロード ** //

		// イージングの簡略化のため実装
		WorldTransform operator+(const WorldTransform& other) const;
		WorldTransform& operator+=(const WorldTransform& other);
		WorldTransform operator/(const float &other) const;

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const WorldTransform& other) const = delete;
		bool operator==(WorldTransform& other) {
			return {
				translation == other.translation &&
				rotation == other.rotation &&
				scale == other.scale &&
				// ペアレントがある場合はペアレント先が変更されていないかも検知
				(parent_ && !parent_->GetChanged())
			};
		}
	};
}