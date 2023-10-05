#pragma once
#include "../math/Math.h"

namespace LWP::Object {
	class WorldTransform {
	public:
		// ** 変数 ** //
 		
		// ローカルスケール
		Math::Vector3 scale;
		// X,Y,Z軸回りのローカル回転角
		Math::Vector3 rotation;
		// ローカル座標
		Math::Vector3 translation;


	private: // ** プロパティ変数 ** //
		// 親となるワールド変換へのポインタ（読み取り専用）
		WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		// 親関係を登録
		void Parent(WorldTransform* parent);
		// 親のポインタを受け取る（実装禁止）
		//WorldTransform* Parent();

	private:
		// CommandManager上でのインデックス
		int index;
	public: // アクセッサ
		// CommandManager上でのインデックスを読みとる
		int GetIndex() const { return index; }
		// 別のMaterialと同期する
		void SetIndex(int value) { index = value; }

	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
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
	};
}