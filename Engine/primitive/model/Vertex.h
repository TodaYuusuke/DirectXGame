#pragma once
#include "math/vector/Vector2.h"
#include "math/vector/Vector3.h"
#include "utility/Color.h"
#include <array>

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace LWP::Primitive {
	/// <summary>
	/// 頂点クラス
	/// </summary>
	class Vertex final {
	public: // ** パブリックなメンバ変数 ** //
		static const uint32_t kNumMaxInfluence = 4;

		Math::Vector3 position = { 0.0f,0.0f,0.0f };// 座標
		Math::Vector2 texCoord = { 0.0f,0.0f };		// UV座標
		Math::Vector3 normal = { 0.0f,0.0f,0.0f };	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色
		std::array<float, Primitive::Vertex::kNumMaxInfluence> weight{};	// 重み
		std::array<int32_t, Primitive::Vertex::kNumMaxInfluence> jointIndices{};	// 影響を受けるジョイント番号
		uint32_t materialIndex = 0;

	public: // ** メンバ関数 ** //

		Vertex() = default;
		~Vertex() = default;


	public: // ** オペレータオーバーロード ** //

		/// <summary>
		/// Observerクラス用のオペレーターオーバーロード
		/// </summary>
		bool operator==(const Vertex& other) const {
			return { position == other.position && texCoord == other.texCoord && normal == other.normal };
		}
	};
}

// 頂点構造体
namespace LWP::Base {
	struct VertexStruct {
		Math::Vector4 position_;	// 座標
		Math::Vector2 texCoord_;	// UV座標
		Math::Vector3 normal_;		// 法線
		Math::Vector4 color_;	// 色
		std::array<float, Primitive::Vertex::kNumMaxInfluence> weight{};	// 重み
		std::array<int32_t, Primitive::Vertex::kNumMaxInfluence> jointIndices{};	// 影響を受けるジョイント番号
		uint32_t materialIndex_;	// 適応されるマテリアルの番号

		VertexStruct() = default;
		VertexStruct(const Primitive::Vertex& value) {
			*this = value;
		}
		// IPrimitiveのVertexを代入する演算子をオーバーロード
		VertexStruct& operator=(const Primitive::Vertex& value) {
			position_ = { value.position.x,value.position.y,value.position.z,1.0f };
			texCoord_ = value.texCoord;
			normal_ = value.normal;
			color_ = value.color.GetVector4();
			weight = value.weight;
			jointIndices = value.jointIndices;
			materialIndex_ = value.materialIndex;
			return *this;
		}
	};
}
