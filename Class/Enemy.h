#pragma once
#include "../Adapter/LWP.h"

class Enemy final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private: // ** メンバ変数 ** //

	// 敵のモデル
	LWP::Primitive::Mesh* model_;
	// 敵のHP
	int32_t hp_;
	// 敵の座標
	LWP::Object::WorldTransform* worldTransform_ = nullptr;
	// 敵の生存フラグ( true == 生存 )
	bool isActive;
	// 敵の浮遊フラグ( true == 浮遊してる )
	bool isFloat;
};