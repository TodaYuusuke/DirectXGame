#pragma once
#include "../Engine/primitive/IPrimitive.h"
#include "../Engine/primitive/Node.h"

#include <memory>

namespace LWP::Base {
	// インデックス情報の構造体
	struct IndexInfoStruct {
		uint32_t vertex;		// 実質頂点インデックスの役割
		uint32_t worldMatrix;	// ワールドトランスフォーム
		uint32_t material;		// マテリアル
		uint32_t tex2d;			// テクスチャ
		uint32_t isUI;			// UIとして表示するかのフラグ
	};

	// 全体で共通のデータ
	struct CommonStruct {
		Math::Matrix4x4 vp2D;		// 2D用のViewProjection
		uint32_t directionLight;    // 平行光源の数
		uint32_t pointLight;        // 点光源の数
	};

#pragma region 頂点データ
	// 頂点データの構造体
	struct VertexStruct {
		Math::Vector4 position_;	// 座標
		Math::Vector2 texCoord_;	// UV座標
		Math::Vector3 normal_;		// 法線
		Math::Vector4 color_;	// 色

		// IPrimitiveのVertexを代入する演算子をオーバーロード
		VertexStruct& operator=(const Primitive::Vertex& value) {
			position_ = { value.position.x,value.position.y,value.position.z,1.0f };
			texCoord_ = value.texCoord;
			normal_ = value.normal;
			color_ = value.color.GetVector4();
			return *this;
		}
	};
#pragma endregion

#pragma region マテリアル
	// マテリアル構造体
	struct MaterialStruct {
		Math::Matrix4x4 uvMatrix;
		int32_t enableLighting;
		float shininess;

		// Materialクラスのデータを代入する演算子をオーバーロード
		MaterialStruct& operator=(const Resource::Material& value) {
			uvMatrix = value.uvTransform.GetWorldMatrix();
			enableLighting = static_cast<int32_t>(value.enableLighting);
			shininess = value.shininess;
			return *this;
		}
	};
#pragma endregion

#pragma region カメラ

#pragma endregion

#pragma region ワールドトランスフォーム
	// ワールドトランスフォーム構造体
	struct WTFStruct {
		Math::Matrix4x4 wtf;
		Math::Matrix4x4 translate;
		Math::Matrix4x4 rotate;
		Math::Matrix4x4 scale;
		Math::Matrix4x4 inverse;

		// ワールドトランスフォームクラスのデータを代入する演算子をオーバーロード
		WTFStruct& operator=(const Object::TransformEuler& value) {
			wtf = value.GetWorldMatrix();
			wtf = value.GetWorldMatrix(new Primitive::Node);
			translate = value.GetTranslationMatrix();
			rotate = value.GetRotateMatrix();
			scale = value.GetScaleMatrix();
			inverse = value.GetWorldMatrix().Inverse();
			return *this;
		}
		// ワールドトランスフォームのデータを代入する演算子をオーバーロード
		WTFStruct& operator=(Primitive::IPrimitive& value) {
			wtf = value.transform.GetWorldMatrix(&value.node);
			translate = value.transform.GetTranslationMatrix();
			rotate = value.transform.GetRotateMatrix();
			scale = value.transform.GetScaleMatrix();
			inverse = value.transform.GetWorldMatrix().Inverse();
			return *this;
		}
	};
#pragma endregion
}