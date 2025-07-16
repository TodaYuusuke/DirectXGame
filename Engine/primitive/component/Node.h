#pragma once
#include "object/TransformQuat.h"

#include <vector>
#include <string>

// 前方宣言
struct aiNode;

namespace LWP::Primitive {

	/// <summary>
	/// ノード情報を格納するクラス
	/// </summary>
	class Node {
	public: // ** パブリックなメンバ変数 ** //

		// トランスフォーム（クォータニオン）
		Object::TransformQuat transform;
		// Nodeの名前
		std::string name = "";
		// 子供のNode
		std::vector<Node> children;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Node() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Node() = default;

		/// <summary>
		/// AssimpのNodeデータから情報を読み込む
		/// </summary>
		/// <param name="material"></param>
		void Load(aiNode* node);

		/// <summary>
		/// LocalMatrixを求める関数
		/// </summary>
		/// <returns></returns>
		Math::Matrix4x4 GetLocalMatrix() const ;
	};
}