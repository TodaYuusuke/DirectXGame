#pragma once
#include "resources/model/RigidModel.h"

namespace LWP::Primitive {
	/// <summary>
	/// 3DのPrimitiveの基底クラス（親はRigidModelを利用する。※ AmpShader）
	/// </summary>
	class IPrimitive3D
		: public Resource::RigidModel {
	public: // ** パブリックなメンバ変数 ** //
		// 形状
		enum Type {
			Cube,
			Sphere,
			Cylinder,
			Capsule,
			Count
		};

		/// <summary>
		/// コンストラクタ
		/// </summary>
		IPrimitive3D();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~IPrimitive3D();

		/// <summary>
		/// 形状を取得
		/// </summary>
		/// <returns></returns>
		virtual Type GetType() = 0;

		// 名前
		std::string name;


	protected: // ** プライベートな関数 ** //

		/// <summary>
		/// 3Dモデルのデータを読み込む
		/// </summary>
		/// <param name="fileName">読み込むファイルの名前</param>
		void Load(const std::string& fileName);
		/// <summary>
		/// 3Dモデルのデータを読み込む（resources/model/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadShortPath(const std::string& filePath);
		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		using RigidModel::LoadFullPath;
	};
}