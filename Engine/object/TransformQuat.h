#pragma once
#include "../math/Math.h"
#include <string>
#include "utility/observers/Observer.h"

// 前方宣言
namespace LWP::Primitive {
	class Node;
}

namespace LWP::Object {
	class TransformQuat {
	public: // ** パブリックな変数 ** //
		// ローカル座標
		Math::Vector3 translation;
		// X,Y,Z軸回りのローカル回転角
		Math::Quaternion rotation;
		// ローカルスケール
		Math::Vector3 scale;


	private: // ** プロパティ変数 ** //
		// 親となるワールド変換へのポインタ（読み取り専用）
		Utility::Observer<TransformQuat*>* parent_ = nullptr;
		//WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		// 親関係を登録
		void Parent(TransformQuat* parent);
		// 親のポインタを受け取る（実装禁止）
		//WorldTransform* Parent();
		

	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		TransformQuat();
		TransformQuat(Math::Vector3 t, Math::Quaternion r, Math::Vector3 s);
		TransformQuat(Math::Vector3 t, Math::Quaternion r);
		TransformQuat(Math::Vector3 t);
		~TransformQuat() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// ワールド行列を返す
		/// </summary>
		Math::Matrix4x4 GetWorldMatrix() const;
		/// <summary>
		/// ワールド行列を返す（Nodeを考慮）
		/// </summary>
		Math::Matrix4x4 GetWorldMatrix(Primitive::Node* node) const;

		// 移動だけのワールド行列を返す
		Math::Matrix4x4 GetTranslationMatrix() const;
		// ワールド座標を返す
		Math::Vector3 GetWorldPosition() const;

		// 回転だけのワールド行列を返す
		Math::Matrix4x4 GetRotateMatrix() const;
		// 最終的な回転を返す
		//Math::Vector3 GetFinalRotate() const;

		// 拡大縮小だけのワールド行列を返す
		Math::Matrix4x4 GetScaleMatrix() const;
		// 最終的なスケールを返す
		Math::Vector3 GetFinalScale() const;

	

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
		TransformQuat operator+(const TransformQuat& other) const;
		TransformQuat& operator+=(const TransformQuat& other);
		//WorldTransform operator/(const float &other) const;

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const TransformQuat& other) const = delete;
		bool operator==(TransformQuat& other) {
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