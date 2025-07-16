#include "ResourceManager.h"

#include "math/Math.h"
#include "utility/ErrorReporter.h"
#include "base/DirectXCommon.h"

#include "component/Resource.h"

#include <fstream>
#include <sstream>


using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;

InstanceData::InstanceData(const Resource::RigidModel& value) {
	*this = value;
}
InstanceData& InstanceData::operator=(const Resource::RigidModel& value) {
	wtf = value.worldTF;
	return *this;
}
InstanceData::InstanceData(const Resource::SkinningModel& value) {
	*this = value;
}
InstanceData& InstanceData::operator=(const Resource::SkinningModel& value) {
	wtf = value.worldTF;
	return *this;
}
InstanceData::InstanceData(const Resource::EMapModel& value) {
	*this = value;
}
InstanceData& InstanceData::operator=(const Resource::EMapModel& value) {
	wtf = value.worldTF;
	return *this;
}


void Models::RigidBuffer::Init() {
	inst = std::make_unique<Base::StructuredBuffer<Base::InstanceData>>(lwpC::Rendering::kMaxModelInstance);
	inst->Init();
	material = std::make_unique<Base::StructuredBuffer<Base::MaterialStruct>>(lwpC::Rendering::kMaxMaterial);
	material->Init();
	common.Init();
}
void Models::RigidBuffer::Reset(uint32_t mSize) {
	inst->Reset();
	material->Reset();
	mSize;
	//common.data_->materialCount = mSize;
	common.data_->instanceCount = 0;
}

void Models::SkinBuffer::Init() {
	inst = std::make_unique<Base::StructuredBuffer<Base::InstanceData>>(lwpC::Rendering::kMaxModelInstance);
	inst->Init();
	material = std::make_unique<Base::StructuredBuffer<Base::MaterialStruct>>(lwpC::Rendering::kMaxMaterial);
	material->Init();
	well = std::make_unique<Base::StructuredBuffer<Primitive::WellForGPU>>(lwpC::Rendering::kMaxSkinJointInstance);
	well->Init();
	common.Init();
}
void Models::SkinBuffer::Reset(uint32_t mSize) {
	inst->Reset();
	material->Reset();
	well->Reset();
	mSize;
	//common.data_->materialCount = mSize;
	common.data_->instanceCount = 0;
	//common.data_->jointCount = 0;
}


Manager::~Manager() {
	// XAudio2解放
	xAudio2_.Reset();
	// Media Foundation
	MFShutdown();
	// テクスチャ解放
	textureMap_.clear();
	// オーディオ解放
	audioMap_.clear();
	// モデル解放
	modelDataMap_.clear();
}

void Manager::Init() {
	HRESULT hr;

	// XAudio2初期化
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	// MediaFoundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	for (IModel* m : debugModels) {
		delete m;
	}
	debugModels.clear();
}

void Manager::Update() {
	// アニメーション更新
	for (Animation* a : animations_.list) { a->Update(); }
	// モーション更新
	for (Motion* m : motions_.list) { m->Update(); }
	// モデルアダプター更新
	for (std::map<std::string, Models>::iterator it = modelDataMap_.begin(); it != modelDataMap_.end(); ++it) {
		// リソースリセット
		it->second.rigid.Reset(static_cast<uint32_t>(it->second.data.materials_.size()));
		it->second.skin.Reset(static_cast<uint32_t>(it->second.data.materials_.size()));
		it->second.eMaps.Reset(static_cast<uint32_t>(it->second.data.materials_.size()));


		Models::Pointers<RigidModel, Models::RigidBuffer>* rigid[2] = { &it->second.rigid.solid, &it->second.rigid.wireFrame };
		Models::Pointers<SkinningModel, Models::SkinBuffer>* skin[2] = { &it->second.skin.solid, &it->second.skin.wireFrame };
		Models::Pointers<EMapModel, Models::RigidBuffer>* emap = &it->second.eMaps;
		for (int i = 0; i < 2; i++) {
			for (RigidModel* m : rigid[i]->ptrs.list) {
				m->Update();
				// バッファーにデータ登録
				if (m->isActive) {
					rigid[i]->buffer.inst->Add(*m);
					// 順番に追加する
					for (std::string str : it->second.data.materialOrder_) {
						rigid[i]->buffer.material->Add(m->materials[str]);
					}
					rigid[i]->buffer.common.data_->instanceCount += 1;
				}
			}
			for (SkinningModel* m : skin[i]->ptrs.list) {
				m->Update();
				// バッファーにデータ登録
				if (m->isActive) {
					skin[i]->buffer.inst->Add(*m);
					// 順番に追加する
					for (std::string str : it->second.data.materialOrder_) {
						skin[i]->buffer.material->Add(m->materials[str]);
					}
					m->SetBufferData(skin[i]->buffer.well.get());
					skin[i]->buffer.common.data_->instanceCount += 1;
				}
			}
		}

		for (EMapModel* e : emap->ptrs.list) {
			e->Update();
			// バッファーにデータ登録
			if (e->isActive) {
				emap->buffer.inst->Add(*e);
				// 順番に追加する
				for (std::string str : it->second.data.materialOrder_) {
					emap->buffer.material->Add(e->materials[str]);
				}
				emap->buffer.common.data_->instanceCount += 1;
			}
		}
	}
}

Texture Manager::LoadTexture(const std::string& filepath) {
	return LoadTextureLongPath(textureDirectoryPath_ + filepath);
}
Texture Manager::LoadTextureLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!textureMap_.count(filepath)) {
		// 新しいテクスチャをロード
		textureMap_[filepath] = TextureStruct();
		// 読み込み
		textureMap_[filepath].r.Init(filepath);
		// インデックスを保持
		textureMap_[filepath].tex = textureMap_[filepath].r;
	}
	return textureMap_[filepath].tex;
}

AudioData* Manager::LoadAudio(const std::string& filepath) {
	return LoadAudioLongPath(audioDirectoryPath_ + filepath);
}
AudioData* Manager::LoadAudioLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!audioMap_.count(filepath)) {
		// 新しいテクスチャをロード
		audioMap_[filepath] = AudioData(xAudio2_.Get(), filepath);
	}
	return &audioMap_[filepath];
}

// モデルのデータを読み込む関数
void Manager::LoadModelData(const std::string& filePath) {
	// 読み込み済みかをチェック
	if (!modelDataMap_.count(filePath)) {
		// 読み込んだことのない3Dモデルなので読み込む
 		modelDataMap_[filePath].data.Load(filePath);	// 要素は自動追加されるらしい;

		// 初期化
		modelDataMap_[filePath].rigid.Init();
		modelDataMap_[filePath].skin.Init();
		modelDataMap_[filePath].eMaps.Init();

		// マテリアルの数とJointの数をここでセット
		int m = static_cast<int>(modelDataMap_[filePath].data.materials_.size());
		int j = static_cast<int>(modelDataMap_[filePath].data.skeleton_->joints.size());
		modelDataMap_[filePath].rigid.solid.buffer.common.data_->materialCount = m;
		modelDataMap_[filePath].rigid.wireFrame.buffer.common.data_->materialCount = m;
		modelDataMap_[filePath].skin.solid.buffer.common.data_->materialCount = m;
		modelDataMap_[filePath].skin.wireFrame.buffer.common.data_->materialCount = m;
		modelDataMap_[filePath].skin.solid.buffer.common.data_->jointCount = j;
		modelDataMap_[filePath].skin.wireFrame.buffer.common.data_->jointCount = j;
		modelDataMap_[filePath].eMaps.buffer.common.data_->materialCount = m;
	}
}
ModelData* Manager::GetModelData(const std::string& filePath) {
	// 読み込み済みかをチェック
	if (modelDataMap_.count(filePath)) {
		// 読み込み済みだったので返す
		return &modelDataMap_[filePath].data;
	}

	// 読み込めていないモデルなのでエラー
	assert(false);
	return nullptr;
}
std::vector<std::reference_wrapper<Models>> Manager::GetModels() {
	// ファイルパスのkeyはいらないのでデータだけ渡す
	std::vector<std::reference_wrapper<Models>> models;
	for (auto& pair : modelDataMap_) {
		models.push_back(std::ref(pair.second));
	}
	return models;
}

void Manager::ChangeFillMode(RigidModel* ptr, const std::string& filePath) {
	Models::FillMode<RigidModel, Models::RigidBuffer>& f = modelDataMap_[filePath].rigid;

	// solidの方にあればwireframeに
	if (f.solid.ptrs.Find(ptr)) {
		f.solid.ptrs.DeletePtr(ptr);
		f.wireFrame.ptrs.SetPtr(ptr);
	}
	// wireframeの方にあればsolidに
	else {
		f.wireFrame.ptrs.DeletePtr(ptr);
		f.solid.ptrs.SetPtr(ptr);
	}
}
void Manager::ChangeFillMode(SkinningModel* ptr, const std::string& filePath) {
	Models::FillMode<SkinningModel, Models::SkinBuffer>& f = modelDataMap_[filePath].skin;

	// solidの方にあればwireframeに
	if (f.solid.ptrs.Find(ptr)) {
		f.solid.ptrs.DeletePtr(ptr);
		f.wireFrame.ptrs.SetPtr(ptr);
	}
	// wireframeの方にあればsolidに
	else {
		f.wireFrame.ptrs.DeletePtr(ptr);
		f.solid.ptrs.SetPtr(ptr);
	}
}

void Manager::RigidGUI(Models& m) {
	static int selectedNum = 0;	// 選択された番号
	// solidとwireFrameをまとめたリスト
	std::list<RigidModel*> lists = Utility::MergeLists<RigidModel*>(m.rigid.solid.ptrs.list, m.rigid.wireFrame.ptrs.list);
	int size = static_cast<int>(lists.size());

	// Rigidの一覧を表示
	if (!lists.empty()) {
		// 最大値以上にならないように修正
		if (selectedNum >= size) {
			selectedNum = size - 1;
		}

		std::string text = "";
		for (int i = 0; i < size; i++) {
			text += std::to_string(i);
			text += '\0';
		}
		text += '\0';	// 最後は二個必要なので追加
		ImGui::Combo("List", &selectedNum, text.c_str());

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<RigidModel*>(lists, selectedNum))->DebugGUI();
	}
	ImGui::Text("--------- Solid Info ---------");
	ImGui::Text("ptr Count ... %d", m.rigid.solid.ptrs.list.size());
	ImGui::Text("InstanceData UsedCount ... %d", m.rigid.solid.buffer.inst->GetCount());
	ImGui::Text("MaterialStruct UsedCount ... %d", m.rigid.solid.buffer.material->GetCount());
}
void Manager::SkinningGUI(Models& m) {
	static int selectedNum = 0;	// 選択された番号
	// solidとwireFrameをまとめたリスト
	std::list<SkinningModel*> lists = Utility::MergeLists<SkinningModel*>(m.skin.solid.ptrs.list, m.skin.wireFrame.ptrs.list);
	int size = static_cast<int>(lists.size());

	// Skinningの一覧を表示
	if (!lists.empty()) {
		// 最大値以上にならないように修正
		if (selectedNum >= size) {
			selectedNum = size - 1;
		}

		std::string text = "";
		for (int i = 0; i < size; i++) {
			text += std::to_string(i);
			text += '\0';
		}
		text += '\0';	// 最後は二個必要なので追加
		ImGui::Combo("List", &selectedNum, text.c_str());

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<SkinningModel*>(lists, selectedNum))->DebugGUI();
	}
}
void Manager::StaticGUI(Models& m) {
	static int selectedNum = 0;	// 選択された番号
	std::list<StaticModel*>& list = m.statics.list;
	
	// Staticの一覧を表示
	if (!list.empty()) {
		int size = static_cast<int>(list.size());
		// 最大値以上にならないように修正
		if (selectedNum >= size) {
			selectedNum = size - 1;
		}

		std::string text = "";
		for (int i = 0; i < size; i++) {
			text += std::to_string(i);
			text += '\0';
		}
		text += '\0';	// 最後は二個必要なので追加
		ImGui::Combo("List", &selectedNum, text.c_str());

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<StaticModel*>(list, selectedNum))->DebugGUI();
	}
}


void Manager::DebugGUI() {
	// ImGuiを表示

	// 生成用の関数ポインタ
	static std::vector<std::function<void(std::string)>> functions = {
		[this](std::string str) {
			debugModels.push_back(new RigidModel());
			debugModels.back()->LoadFullPath(str);
		},
		[this](std::string str) {
			debugModels.push_back(new SkinningModel());
			debugModels.back()->LoadFullPath(str);
		},
		[this](std::string str) {
			debugModels.push_back(new StaticModel());
			debugModels.back()->LoadFullPath(str);
		},
	};

	if (ImGui::BeginTabItem("Resource")) {
		// 読み込み済みのモデル一覧
		if (!modelDataMap_.empty()) {
			std::vector<const char*> itemText;
			float maxSize = 0;	// Textの中で一番長いサイズを保持する
			// 一覧のパス取得
			for (std::map<std::string, Models>::iterator it = modelDataMap_.begin(); it != modelDataMap_.end(); ++it) {
				itemText.push_back(it->first.c_str());
				maxSize = std::max<float>(maxSize, static_cast<float>(it->first.size()));	// 現在の長さより大きければ更新
			}
			ImGui::PushItemWidth(maxSize * 7.5f);	// 最大サイズにUIを合わせる
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			// 現在選択中のModelの参照を取得
			auto m = Utility::GetIteratorAtIndex<std::string, Models>(modelDataMap_, currentItem);

			// どっち（rigid,skinning）を表示するか選択
			ImGui::RadioButton("Rigid", &radioValue, 0);
			ImGui::RadioButton("Skinning", &radioValue, 1);
			ImGui::RadioButton("Static", &radioValue, 2);

			// 変更されて渡される値は添え字
			if (ImGui::Button("Create")) { functions[radioValue](m->first); }

			switch (radioValue) {
				default:
				case 0:
					RigidGUI(m->second);
					break;
				case 1:
					SkinningGUI(m->second);
					break;
				case 2:
					StaticGUI(m->second);
					break;
			}
		}
		ImGui::EndTabItem();
	}

	// ** アニメーションクラス ** //

	if (ImGui::BeginTabItem("Animation")) {
		// 読み込み済みのアニメーション一覧
		if (!animations_.list.empty()) {
			std::vector<const char*> itemText;
			for (Animation* p : animations_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentAnim, itemText.data(), static_cast<int>(itemText.size()), 4);
			// 現在選択中のアニメーションのDebugGUIを呼び出し
			(*Utility::GetIteratorAtIndex<Animation*>(animations_.list, currentAnim))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}