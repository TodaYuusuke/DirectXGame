#pragma once
#include "../Engine/primitive/IPrimitive.h"	// 頂点
//#include "../Engine/resources/material/Material.h"		// マテリアル
//#include "../Engine/object/WorldTransform.h"	// 定数
//#include "../Engine/resources/texture/Texture.h"	// テクスチャ

namespace LWP::Base {
	// インデックス情報の構造体
	struct IndexInfoStruct {
		uint32_t vertex;        // 実質頂点インデックスの役割
		uint32_t cameraVP;      // カメラのビュープロジェクション行列
		uint32_t worldMatrix;   // ワールドトランスフォーム
		uint32_t material;      // マテリアル
		uint32_t tex2d;         // テクスチャ
	};
	// インデックスバッファー
	struct IndexResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
		IndexInfoStruct* data_;
		UINT usedCount_ = 0;
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
	// 頂点データのバッファー
	struct VertexResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	// GPU上の頂点データの格納場所
		D3D12_GPU_DESCRIPTOR_HANDLE view_{};			// BufferLocationは頂点データ格納場所のアドレス
		VertexStruct* data_ = nullptr;	// 頂点リソース
		UINT usedCount_ = 0;	// 使用済みのインデックスをカウント
	};
#pragma endregion

#pragma region マテリアル
	// マテリアル構造体
	struct MaterialStruct {
		Math::Matrix4x4 uvMatrix;
		int32_t enableLighting;

		// Materialクラスのデータを代入する演算子をオーバーロード
		MaterialStruct& operator=(const Resource::Material& value) {
			uvMatrix = value.uvTransform.GetWorldMatrix();
			enableLighting = static_cast<int32_t>(value.enableLighting);
			return *this;
		}
	};
	// マテリアルバッファー
	struct MaterialResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
		MaterialStruct* data_;
		UINT usedCount_ = 0;	// 使用済みのインデックスをカウント
	};
#pragma endregion

#pragma region 行列
	// 行列データのバッファー
	struct MatrixResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
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

#pragma region 平行光源
	// 平行光源の構造体
	struct DirectionalLight {
		Math::Matrix4x4 viewProjection_;	// 平行射影行列
		Math::Vector4 color_;		// ライトの色
		Math::Vector3 direction_;	// ライトの向き
		float intensity_;	// 輝度
	};
	// 平行光源のバッファ
	struct LightResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	// 3D用の定数バッファ
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		DirectionalLight* data_ = nullptr;	// 2D用の定数リソース
		Math::Vector3 translation_;	// 光源の位置
		Math::Vector3 rotation_;	// 光源の回転向き
	};
#pragma endregion
}