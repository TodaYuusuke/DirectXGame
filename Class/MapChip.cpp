#include "MapChip.h"

////MapChip::MapChip(LWP::Primitive::Mesh* model, LWP::Math::Vector3 position, uint32_t textureHandle) 
////{
////	this->Initialize(model, position, textureHandle);
////}
////
////MapChip::~MapChip(){}

void MapChip::Initialize(LWP::Math::Vector3 position)
{
	if (!isActive_) { return; }

	isActive_ = false;

	lineCount_ = 0;

	worldTransform_.translation = position;
	
	testMapModel_ = LWP::Resource::LoadModel("cube.obj");
	
	// マップチップのスケールを設定
	testMapModel_->transform.scale = { 0.2f, 0.2f, 0.2f };

	mapCSV = ("resources/csv/mapData.csv");
	LoadMap(mapCSV);

}

void MapChip::Update()
{ 
	if (!isActive_) { return; }
		UpdataMapData();

}

void MapChip::Draw() { 
	if (!isActive_) { return; }
	testMapModel_->isActive = true;
}

void MapChip::LoadMap(const char* fileName) {
	mapFile_.open(fileName);
	assert(mapFile_.is_open());

	mapBoxCreateCommands_ >> mapFile_.rdbuf();

	// ファイルを閉じる
	mapFile_.close();

}

void MapChip::UpdataMapData()
{

	// CVSファイルの最後の行までループを回す
	while (std::getline(mapBoxCreateCommands_,line_)) {
		// 読み込んだ一行をカンマで区切り代入
		std::vector<std::string> strvec = split(line_, ',');

		// 一行のサイズ分繰り返し文を回す
		for (int colmnCount = 0; colmnCount < strvec.size(); colmnCount++) {

			// CSVファイルの中の対応する数字を見つけたらマップを生成する
			if (stoi(strvec.at(colmnCount)) == 1) {

				// 座標を決めてブロックを生成
				CreateMap(CreateMapVector(colmnCount, lineCount_));
			}
		}
		// カウントに1を足す
		lineCount_ = lineCount_ + 1;
	}
}

LWP::Math::Vector3 MapChip::CreateMapVector(int indexX, int indexY)
{
	LWP::Math::Vector3 result{};

	result.x = mapManager_->GetMapBoxSize() * float(indexX);
	result.y = mapManager_->GetMapBoxSize() * float(-indexY);
	result.z = 0.0f;

	return result;
}

void MapChip::CreateMap(LWP::Math::Vector3 position)
{

	// 新しいマップチップを生成
	MapChip* newMapChip = new MapChip();

	// 初期化
	newMapChip->Initialize(position);

	//生成したマップチップをmapXector_に格納
	mapVector_.push_back(newMapChip);

}

std::vector<std::string> MapChip::split(std::string& input, char delimiter) {
	std::istringstream stream(input);
	std::string field;
	std::vector<std::string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

