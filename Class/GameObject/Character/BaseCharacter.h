#pragma once
#include <map>
#include "Model.h"
#include "../Collision/Collider.h"

class BaseCharacter : public Collider {
public: // メンバ関数

	// 初期化
	virtual void Initialize(const std::vector<Model*>& models, Vector3 position);
	// 更新
	virtual void Update();
	// 描画
	virtual void Draw(const ViewProjection& viewProjection);

protected: // メンバ変数
	// モデルデータとモデル別のワールド変換データ
	struct ModelData {
		Model* model_;
		WorldTransform worldTransform_;
	};

	// モデルデータ配列
	std::map<int, ModelData> models_;
};