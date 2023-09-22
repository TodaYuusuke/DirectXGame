#pragma once
#include "../object/WorldTransform.h"
#include "../math/Vector3.h"
#include "../math/Vector2.h"
#include "../resources/texture/Texture.h"
#include "../resources/Material.h"
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

	struct Vertex {
		Math::Vector3 position;	// 座標
		Math::Vector2 texCoord; // UV座標
		Math::Vector3 normal;	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色
	};


	class IPrimitive {
	public:
		// ** 共通の変数 ** //

		// ワールドトランスフォーム
		Object::WorldTransform transform;

		// 頂点共通のカラー
		// ・nullptrの場合は頂点ごとに色を参照する
		// ・そうでない場合は全ての頂点の色がこれになる
		Utility::Color* defaultColor = nullptr;
		// マテリアル
		Resource::Material* material = nullptr;

		// UIとして描画するか
		bool isUI = false;

	protected:	// 描画用にマネージャーのポインタを保持する
		Manager* primitiveManager;

		
	public:	// ** 純粋仮想関数 ** //

		// 描画
		virtual void Draw(FillMode fillmode, Resource::Texture* texture = nullptr) = 0;
		// 頂点数を返す関数
		virtual int GetVertexCount() const = 0;

	};
}