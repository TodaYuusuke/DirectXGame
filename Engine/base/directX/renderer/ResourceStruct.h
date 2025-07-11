#pragma once
#include "primitive/component/Vertex.h"
#include "primitive/component/Material.h"

#include "../Engine/primitive/IPrimitive.h"

#include <memory>

namespace LWP::Base {
	// インデックス情報の構造体
	struct IndexInfoStruct {
		uint32_t vertex;		// 実質頂点インデックスの役割
		uint32_t worldMatrix;	// ワールドトランスフォーム
		uint32_t material;		// マテリアル
		uint32_t isUI;			// スクリーン座標で表示するかのフラグ
	};

	// 全体で共通のデータ
	struct CommonStruct {
		Math::Matrix4x4 vp2D;		// 2D用のViewProjection
		uint32_t directionLight;    // 平行光源の数
		uint32_t pointLight;        // 点光源の数
	};



#pragma region ワールドトランスフォーム
	// ワールドトランスフォーム構造体
	struct WTFStruct {
		Math::Matrix4x4 wtf;
		Math::Matrix4x4 translate;
		Math::Matrix4x4 rotate;
		Math::Matrix4x4 scale;
		Math::Matrix4x4 inverse;

		// TransformEulerクラスのデータを代入する演算子をオーバーロード
		WTFStruct& operator=(const Object::TransformEuler& value) {
			wtf = value.GetAffineMatrix();
			//wtf = value.GetAffineMatrix(new Primitive::Node);
			translate = value.GetTranslationMatrix();
			rotate = value.GetRotateMatrix();
			scale = value.GetScaleMatrix();
			inverse = value.GetAffineMatrix().Inverse();
			return *this;
		}
		// TransformQuatクラスのデータを代入する演算子をオーバーロード
		WTFStruct& operator=(const Object::TransformQuat& value) {
			wtf = value.GetAffineMatrix();
			//wtf = value.GetAffineMatrix(new Primitive::Node);
			translate = value.GetTranslationMatrix();
			rotate = value.GetRotateMatrix();
			scale = value.GetScaleMatrix();
			inverse = value.GetAffineMatrix().Inverse();
			return *this;
		}
		// ワールドトランスフォームのデータを代入する演算子をオーバーロード
		WTFStruct& operator=(Primitive::IPrimitive& value) {
			wtf = value.worldTF.GetAffineMatrix();
			translate = value.worldTF.GetTranslationMatrix();
			rotate = value.worldTF.GetRotateMatrix();
			scale = value.worldTF.GetScaleMatrix();
			inverse = value.worldTF.GetAffineMatrix().Inverse();
			return *this;
		}

		WTFStruct() = default;
		WTFStruct(const Object::TransformQuat& value) {
			*this = value;
		}
	};

#pragma endregion
};
