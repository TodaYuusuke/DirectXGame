#pragma once
#include "../RigidModel.h"

namespace LWP::Resource {
	/// <summary>
	/// 立方体
	/// </summary>
	class Cube final : public RigidModel {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Cube();
		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		Cube(const Cube& other);

		/// <summary>
		/// デストラクタは基底クラスのものを使用
		/// </summary>

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;


		// コピー演算子
		Cube& operator=(const Cube& other) {
			if (this != &other) {
				worldTF = other.worldTF;
				enableLighting = other.enableLighting;
				isActive = other.isActive;
			}
			return *this;
		}

	private: // ** 読み込み系関数は秘匿 ** //

		/// <summary>
		/// 3Dモデルのデータを読み込む関数
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		using RigidModel::Load;
		using RigidModel::LoadShortPath;
		using RigidModel::LoadFullPath;
	};
}