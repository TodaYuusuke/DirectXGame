#pragma once
#include "object/TransformEuler.h"
#include "resources/texture/Texture.h"

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
		void DebugGUI();


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
		int32_t enableLighting;
		float shininess;

		// Materialクラスのデータを代入する演算子をオーバーロード
		MaterialStruct& operator=(const Primitive::Material& value) {
			uvMatrix = value.uvTransform.GetWorldMatrix();
			shininess = value.shininess;
			return *this;
		}
	};
}