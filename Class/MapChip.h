#pragma once
#include "../Adapter/Adapter.h"
#include "MapManager.h"
#include <vector>
#include <fstream>
#include <sstream>

class MapChip final {
public:
	// ** メンバ関数 ** //

	/*/// <summary>
	/// コンストラクタ
	/// </summary>
	MapChip(LWP::Primitive::Mesh* model, LWP::Math::Vector3 position, uint32_t textureHandle);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapChip();*/

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Math::Vector3 position);
	/// <summary>
	/// 初期化
	/// </summary>
	void Update();
	/// <summary>
	/// 初期化
	/// </summary>
	void Draw();

	/// <summary>
	/// マップ生成スクリプト
	/// </summary>
	void UpdataMapData();

	/// <summary>
	/// マップの座標を決める
	/// </summary>
	LWP::Math::Vector3 CreateMapVector(int indexX, int indexY);

	/// <summary>
	/// マップを生成する
	/// </summary>
	void CreateMap(LWP::Math::Vector3 position);

	void LoadMap(const char* fileName);

	std::vector<std::string> split(std::string& input, char delimiter);

private: // ** メンバ変数 ** //

	// マップチップのモデル
	LWP::Primitive::Mesh* testMapModel_;

	MapManager* map_;

	// ワールド変換データ
	LWP::Object::WorldTransform worldTransform_;

	// テクスチャ
	uint32_t textureHandle_ = 0;

	// 1行分の文字列を入れる変数
	std::string line_;

	// 読み込んでるマップの現在行
	int lineCount_ = 0;

	// マップの座標を入れる変数
	LWP::Math::Vector3 position_{};

	//マップファイル
	std::ifstream mapFile_;

	//マップ生成用コマンド
	std::stringstream mapBoxCreateCommands_;

	//マップデータを格納するベクトル
	std::vector<MapChip*> mapVector_;

};