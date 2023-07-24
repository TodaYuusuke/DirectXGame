#pragma once
#include "../object/WorldTransform.h"
#include "../math/Vector3.h"
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
		Utility::Color color = { 255,255,255,255 };	// 色
	};

	class IPrimitive {
	public:
		//*** 純粋仮想関数 ***//

		// 描画
		virtual void Draw(FillMode fillmode) = 0;
		// 頂点数を返す関数
		virtual int GetVertexCount() const = 0;

		// トランスフォーム
		LWP::Object::WorldTransform transform;

		// 頂点共通のカラー
		// ・nullptrの場合は頂点ごとに色を参照する
		// ・そうでない場合は全ての頂点の色がこれになる
		Utility::Color* defaultColor = nullptr;

	protected:
		Manager* primitiveManager;
	};
}