#pragma once
#include "../math/matrix/Matrix4x4.h"

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

		// NodeのLocalMatrix
		Math::Matrix4x4 localMatrix{};
		// Nodeの名前
		std::string name = "";
		// 子供のNode
		std::vector<Node> children;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Node();

		/// <summary>
		/// assimpのNodeを読み取る関数
		/// </summary>
		void ReadNode(aiNode* node);
	};
}