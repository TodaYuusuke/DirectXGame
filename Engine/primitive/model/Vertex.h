#pragma once
#include "math/vector/Vector2.h"
#include "math/vector/Vector3.h"
#include "math/matrix/Matrix4x4.h"

#include "utility/Color.h"
#include <array>

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// 前方宣言
namespace LWP::Primitive {
	class Vertex;
}
namespace LWP::Base {
	struct VertexStruct;
	struct OutputVertexStruct;
}

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

		// Vertexに戻すためのオペレータオーバーロード
		Vertex& operator=(const Base::OutputVertexStruct& value);
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

	/// <summary>
	/// MSで出力される頂点データの構造体
	/// </summary>
	struct OutputVertexStruct {
		Math::Vector4 position;	// 座標
		Math::Vector3 worldPosition;	// ワールド座標
		Math::Vector2 texCoord;	// UV座標
		Math::Vector3 normal;	// 法線
		Math::Vector4 color;	// 色
		uint32_t mIndex;	// 適応されるマテリアルの番号

		OutputVertexStruct() = default;
		OutputVertexStruct(const VertexStruct& value) {
			*this = value;
		}
		// VertexStructからデータをコピーするためのオペレータオーバーロード
		OutputVertexStruct& operator=(const VertexStruct& value) {
			position = value.position_;
			worldPosition = { 0.0f,0.0f,0.0f };	// 求められないので初期値
			texCoord = value.texCoord_;
			normal = value.normal_;
			color = value.color_;
			mIndex = value.materialIndex_;
			return *this;
		}
		// OutPutVertexStructにWorldTransformを適応する
		void ApplyWorldTransform(const Math::Matrix4x4& affine);
	};
}
