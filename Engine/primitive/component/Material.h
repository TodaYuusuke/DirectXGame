#pragma once
#include "object/TransformEuler.h"
#include "resources/texture/Texture.h"

#include "utility/other/Color.h"

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace LWP::Primitive {
	/// <summary>
	/// マテリアル
	/// </summary>
	class Material final {
	public: // ** パブリックなメンバ変数 ** //

		// ライテイングを行うか
		bool enableLighting = true;
		// カラー
		Utility::Color color = { 255,255,255,255 };
		// uvTransform
		Object::TransformEuler uvTransform;
		// 光沢度
		float shininess = 100000.0f;	// この値にするとほぼ見えない

		// テクスチャ
		Utility::Observer<Resource::Texture> texture;


	public: // ** パブリックなメンバ関数 ** //

		// コンストラクタ
		Material() = default;
		// デストラクタ
		~Material() = default;

		/// <summary>
		/// Assimpのmaterialデータから情報を読み込む
		/// </summary>
		/// <param name="material"></param>
		void Load(aiMaterial* material, const aiScene* scene, const std::string& filePath);

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "Material");


	public: // ** オペレーターオーバーロード ** //

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(Material& other) {
			return {
				uvTransform == other.uvTransform &&
				shininess == other.shininess &&
				texture.GetChanged()
			};
		}

	};
}

// マテリアル構造体
namespace LWP::Base {
	struct MaterialStruct {
		Math::Matrix4x4 uvMatrix;
		Math::Vector4 color;
		int32_t enableLighting;
		float shininess;
		int32_t textureIndex;

		MaterialStruct() = default;
		MaterialStruct(const Primitive::Material& value) {
			*this = value;
		}
		// Materialクラスのデータを代入する演算子をオーバーロード
		MaterialStruct& operator=(const Primitive::Material& value);
	};
}