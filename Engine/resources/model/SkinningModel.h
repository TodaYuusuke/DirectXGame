#pragma once
#include "object/TransformEuler.h"

#include "primitive/model/Skeleton.h"
#include "primitive/model/SkinCluster.h"

#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/renderer/ResourceStruct.h"

// 前方宣言
namespace LWP::Base {
	class RendererManager;
}
namespace LWP::Resource {
	class SkinningModel;
}

// モデル別のデータの構造体（AS実装のときに役立つ予定）
namespace LWP::Base {
	struct InstanceSkinData {
		Base::WTFStruct wtf;
		int32_t enableLighting;

		InstanceSkinData() = default;
		InstanceSkinData(const Resource::SkinningModel& value);
		// Materialクラスのデータを代入する演算子をオーバーロード
		InstanceSkinData& operator=(const Resource::SkinningModel& value);
	};
}

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class SkinningModel final {
	public: // ** パブリックなメンバ変数 ** //

		Object::TransformEuler worldTF{};
		// ワールドトランスフォーム
		// スケルトン
		std::optional<Primitive::Skeleton> skeleton{};
		// スキンクラスター（そのうちModelDataに移植）
		std::optional<Primitive::SkinCluster> skinCluster{};

		// ライティングを行うかどうか
		bool enableLighting = false;
		// ワイヤーフレームで描画する
		//bool isWireFrame = false;
		// アクティブ切り替え
		bool isActive = true;

		Base::ConstantBuffer<Base::InstanceSkinData> buffer;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SkinningModel();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~SkinningModel();

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
		void LoadFullPath(const std::string& filePath);

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		void Update();

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// 読み込み済みのパスを取得
		/// </summary>
		std::string LoadedFilePath() { return filePath; }

	private: // ** メンバ変数 ** //

		// 短縮用パス
		const static std::string kDirectoryPath;
		// リソースマネージャー上のモデルを指す為の変数（パスの予定）
		std::string filePath = "";


	public: // ** 標準的な形状を使いたいとき用のデータ ** //
		/// <summary>
		/// 標準形状一覧
		/// </summary>
		enum class Standard {
			Cube,	// 立方体
			Sphere,	// スフィア
		};
	private: // **  ** //
	};
}