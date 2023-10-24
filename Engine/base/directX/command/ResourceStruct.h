#pragma once
#include "../Engine/primitive/IPrimitive.h"	// 頂点
//#include "../Engine/resources/material/Material.h"		// マテリアル
//#include "../Engine/object/WorldTransform.h"	// 定数
//#include "../Engine/resources/texture/Texture.h"	// テクスチャ

namespace LWP::Base {
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
	// 頂点データとインデックスデータのバッファー
	struct VertexResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;	// GPU上の頂点データの格納場所
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};			// BufferLocationは頂点データ格納場所のアドレス
		VertexStruct* vertexData_ = nullptr;	// 頂点リソース
		int usedVertexCount_ = 0;	// 使用済みのインデックスをカウント
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;	// 頂点データのインデックス格納場所
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};			// BufferLocationはインデックス格納場所のアドレス
		uint32_t* indexData_ = nullptr;	// インデックスリソース
		int usedIndexCount_ = 0;	// 使用済みのインデックスをカウント
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
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		MaterialStruct* data_;
	};
#pragma endregion

#pragma region 行列
	// 行列データのバッファー
	struct MatrixResourceBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_VIRTUAL_ADDRESS view_;
		Math::Matrix4x4* data_;

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
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		D3D12_GPU_DESCRIPTOR_HANDLE view_;
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
	struct LightBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;	// 3D用の定数バッファ
		DirectionalLight* light_ = nullptr;	// 2D用の定数リソース
	};
#pragma endregion
}