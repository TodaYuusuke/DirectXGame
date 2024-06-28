#pragma once
#include "object/TransformEuler.h"

#include "primitive/model/Skeleton.h"
#include "primitive/model/SkinCluster.h"

#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/renderer/ResourceStruct.h"

#include "ModelData.h"

namespace LWP::Resource {

	/// <summary>
	/// 3Dモデルを扱うアダプターの基底クラス
	/// </summary>
	class IModel {
	public: // ** パブリックなメンバ変数 ** //

		// アクティブ切り替え
		bool isActive = true;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IModel() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		virtual ~IModel() = default;

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
		virtual void LoadFullPath(const std::string& filePath) = 0;

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		virtual void Update() = 0;

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		virtual void DebugGUI() = 0;

		/// <summary>
		/// 埋め立てかワイヤーフレームで描画するかを切り替える
		/// </summary>
		virtual void ChangeFillMode() = 0;
		/// <summary>
		/// 読み込み済みのパスを取得
		/// </summary>
		std::string LoadedFilePath() { return filePath; }
		/// <summary>
		/// 自分の読み込んだモデルデータを返す
		/// </summary>
		ModelData* GetModelData() const;

	protected: // ** メンバ変数 ** //

		// 短縮用パス
		const static std::string kDirectoryPath;
		// リソースマネージャー上のモデルを指す為の変数（パスの予定）
		std::string filePath = "";
	};
}