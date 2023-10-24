#pragma once
#include "MapChip.h"
#include "../Mob/IMob.h"

class MapManager final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MapManager() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapManager() = default;

	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 初期化
	/// </summary>
	void Update();


	// ** 外部から操作するための関数 ** //

	/// <summary>
	/// 引数で指定した座標を中心に衝撃波を起こす
	/// </summary>
	/// <param name="centerPosition">衝撃派の中心地点</param>
	/// <returns>衝撃波が発生したときはtrueを返す。ただし範囲外を選択した座標だったときはfalseを返し衝撃波は起こさない</returns>
	bool AddShockWave(LWP::Math::Vector3 centerPosition);

	/// <summary>
	/// マップチップ１つのscaleを求める関数
	/// </summary>
	float GetMapChipScale() { return 2.0f / kSubdivision_; }

	/// <summary>
	/// モブに対しての当たり判定をチェックする関数
	/// </summary>
	void CheckCollision(IMob* mob);

private: // ** メンバ変数 ** //

	// マップの大きさ（半径）
	const float kMapRadius_ = 1.0f;

	// マップの分割数
#if _DEBUG //debug時
	static const int kSubdivision_ = 16;
#else      //release時
	static const int kSubdivision_ = 64;
#endif


	// マップの配列
	MapChip maps_[kSubdivision_][kSubdivision_];

	// 衝撃波最大数
	static const int kMaxShockWaveCount_ = 3;
	// マップに存在する衝撃波の配列
	ShockWave shockWaves[kMaxShockWaveCount_];
	uint8_t shockWaveIndex = 0;
};