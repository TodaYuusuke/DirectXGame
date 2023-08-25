#pragma once
#include "../object/WorldTransform.h"
#include "../math/Vector3.h"
#include "../math/Vector2.h"
#include "../../resources/system/texture/Texture.h"
#include "../../resources/system/Material.h"
#include "../utility/Color.h"
#include <wrl.h>
#include <d3d12.h>

namespace LWP::Primitive {
	// インスタンスをコントローラに格納させるため、コンストラクタでポインタを受け取る
	class Manager;
	enum FillMode : int {
		WireFrame,	// ワイヤーフレーム
		Fill,		// 埋め立て
	};


	struct Vertex2D {
		Math::Vector2 position;	// 座標
		Math::Vector2 texCoord; // UV座標
		Math::Vector3 normal;	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色
	};
	struct Vertex3D {
		Math::Vector3 position;	// 座標
		Math::Vector2 texCoord; // UV座標
		Math::Vector3 normal;	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色

		// Vector2DをVector3Dに変換する
		Vertex3D operator=(const Vertex2D& other) {
			Vertex3D result{};
			result.position = { other.position.x,other.position.y,0.0f };
			result.texCoord = other.texCoord;
			result.normal = other.normal;
			result.color = other.color;
			return result;
		}
	};


	class IPrimitive {
	public:
		//*** 純粋仮想関数 ***//

		// 描画
		virtual void Draw(FillMode fillmode, Resource::Texture* texture = nullptr) = 0;
		// 頂点数を返す関数
		virtual int GetVertexCount() const = 0;

		// トランスフォーム
		Object::WorldTransform transform;

		// 頂点共通のカラー
		// ・nullptrの場合は頂点ごとに色を参照する
		// ・そうでない場合は全ての頂点の色がこれになる
		Utility::Color* defaultColor = nullptr;
		// マテリアル
		Resource::Material* material;

	protected:
		Manager* primitiveManager;
	};
}