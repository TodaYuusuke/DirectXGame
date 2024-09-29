#pragma once
#include <Adapter.h>

class Drone final {
public:

	// 初期化
	void Init(LWP::Object::Camera* ptr, LWP::Object::Terrain* terrain);
	// 更新
	void Update();

	// モデルの状態をセット
	void SetModelTF(LWP::Math::Vector3 pos, LWP::Math::Quaternion rotation);

private: // ** メンバ定数 ** //

	// 速度
	float kSpeed_ = 6.0f;
	// カメラ速度
	float kCameraSpeed_ = 0.05f;
	// 速度の減衰率
	float kDecayRate_ = 0.8f;


private: // ** メンバ変数 ** //

	// 3Dモデル
	LWP::Resource::SkinningModel model_;
	// アニメーション
	LWP::Resource::Animation anim_;
	
	// 速度
	LWP::Math::Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	// 光源
	LWP::Object::PointLight pointLight_;


	// カメラ
	LWP::Object::Camera* cameraPtr_;
	// 地形とのコライダーポインタ
	LWP::Object::Terrain::Point* terrainPoint_;


private: // ** プライベートなメンバ関数 ** //

	// 移動処理
	void Move();
	// 視点操作
	void CameraMove();
};