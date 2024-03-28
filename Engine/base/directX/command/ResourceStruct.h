#pragma once
#include "../Engine/primitive/IPrimitive.h"	// 頂点
#include "object/core/Camera.h"	// カメラ

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
	struct CommonData {
		Math::Matrix4x4 vp2D;		// 2D用のViewProjection
		uint32_t directionLight;    // 平行光源の数
		uint32_t pointLight;        // 点光源の数
	};
	// インデックスバッファー
	struct CommonDataResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		CommonData* data_;
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
	// カメラ構造体
	struct CameraStruct {
		Math::Matrix4x4 viewProjection;
		Math::Matrix4x4 rotate;
		Math::Vector3 position;

		// Materialクラスのデータを代入する演算子をオーバーロード
		CameraStruct& operator=(const Object::Camera& value) {
			viewProjection = value.GetViewProjection();
			rotate = value.transform.GetRotateMatrix();
			position = value.transform.GetWorldPosition();
			return *this;
		}
	};

	// カメラデータのバッファー
	struct CameraResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		CameraStruct* data_;
		UINT usedCount_ = 0;	// 使用済みのインデックスをカウント
	};
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
		WTFStruct& operator=(const Object::WorldTransform& value) {
			wtf = value.GetWorldMatrix();
			translate = value.GetTranslationMatrix();
			rotate = value.GetRotateMatrix();
			scale = value.GetScaleMatrix();
			inverse = value.GetWorldMatrix().Inverse();
			return *this;
		}
	};
#pragma endregion

#pragma region 行列
	// 行列データのバッファー
	struct MatrixResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		Math::Matrix4x4* data_;
		UINT usedCount_ = 0;	// 使用済みのインデックスをカウント

		// WorldTransformの行列を代入する演算子をオーバーロード
		MatrixResourceBuffer& operator=(const Object::WorldTransform& value) {
			*data_ = value.GetWorldMatrix();
			return *this;
		}
	};
#pragma endregion

#pragma region テクスチャ
	// テクスチャのバッファー
	struct TextureResourceBuffer {
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> resource_;
		D3D12_GPU_DESCRIPTOR_HANDLE view_;	// ビューは先頭のテクスチャのみ
		UINT usedCount_ = 0;	// 使用済みのインデックスをカウント
	};
#pragma endregion

#pragma region 光源
	struct DirectionShadowMapStruct {
		// シャドウマップ用テクスチャ
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		// DSVのインデックス
		uint32_t dsvIndex_;
		// シャドウマップのリソースへのビュー
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
	};

	// 平行光源の構造体
	struct DirectionalLightStruct {
		Math::Matrix4x4 vp;		// ViewProjectionをこっちにも登録
		Math::Vector4 color;		// ライトの色
		Math::Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
	};
	// 平行光源のバッファ
	struct DirectionLightResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	// リソース
		D3D12_GPU_DESCRIPTOR_HANDLE view_;	// ビュー
		DirectionalLightStruct* data_ = nullptr;	// 実際のデータ
		UINT usedCount_ = 0;
		Math::Vector3 rotation_;	// 光源の回転向き
		// シャドウマップのリソース
		DirectionShadowMapStruct* shadowMap_ = nullptr;
	};


	struct PointShadowMapStruct {
		// シャドウマップ用テクスチャ
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		// DSVのインデックス
		uint32_t dsvIndex_[6];
		// シャドウマップのリソースへのビュー
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
	};
	// 点光源の構造体
	struct PointLightStruct {
		Math::Vector4 color;	// ライトの色
		Math::Vector3 position;	// ライトのワールド座標
		float intensity;		// 輝度
		float radius;			// ライトの届く最大距離
		float decay;			// 減衰率
	};
	// 点光源のバッファ
	struct PointLightResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	// リソース
		D3D12_GPU_DESCRIPTOR_HANDLE view_;	// ビュー
		PointLightStruct* data_ = nullptr;	// 実際のデータ
		UINT usedCount_ = 0;
		// シャドウマップのリソース
		PointShadowMapStruct* shadowMap_;
	};
#pragma endregion
}