#include "Animation.h"

#include "component/Resource.h"
#include "component/Information.h"
#include "utility/motionEffects/Interpolation.h"

#include "resources/model/SkinningModel.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace LWP::Resource;
using namespace LWP::Primitive;
using namespace LWP::Utility;
using namespace LWP::Math;

Animation::Animation() {
	// インスタンスをエンジンに送る
	SetInstance(this);
}
Animation::~Animation() {
	// エンジン上のポインタを破棄
	DeleteInstance(this);
}

void Animation::Init() {
	time_ = 0.0f;
	playingAnimationName_ = "";
}

void Animation::Play(const std::string animName) { Play(animName, false, 0.0f); }
void Animation::Play(const std::string animName, bool loop) { Play(animName, loop, 0.0f); }
void Animation::Play(const std::string animName, bool loop, float startTime) {
	// 存在するアニメーションかチェック
	if (!data.contains(animName)) {
		// 存在しないのでエラー
		assert(false);
	}

	playingAnimationName_ = animName;
	loopFlag_ = loop;
	time_ = startTime;
	isActive = true;
}
void Animation::Stop() { isActive = false; }

void Animation::Update() {
	// isActiveがfalse、もしくは再生中のアニメーションが""のとき再生しない
	if (!isActive || playingAnimationName_ == "") { return; }	// 早期リターン

	// アニメーションの時間
	float total = data[playingAnimationName_].totalTime;
	// 時間を更新
	//time_ += (useDeltaTimeMultiply ? Info::GetDeltaTimeF() : Info::GetDefaultDeltaTimeF()) / total;	// こちらはデルタタイム更新
	float t = (1.0f / 60.0f) / total;
	if (!reverseFlag_) { time_ += t; }	// リバースフラグに応じて進行方向を変える
	else { time_ -= t; }

	// ループする場合
	if (loopFlag_) {
		// 最後までいったらリピート再生
		if (!reverseFlag_) {
			time_ = std::fmod(time_, 1.0f);
		}
		else {
			if (time_ < 0.0f) { time_ += 1.0f; }	// リバースなので0を下回ったら1を足して戻す
		}
	}
	// ループしない場合
	else {
		// リバースしないなら1を超えないように
		if (!reverseFlag_) {
			if (time_ > 1.0f) { time_ = 1.0f; }
		}
		// リバースするなら0を超えないように
		else {
			if (time_ < 0.0f) { time_ = 0.0f; }
		}
	}

	// アニメーションの時間
	float seconds = total * time_;
	for (Joint& joint : modelPtr_->skeleton.joints) {
		// 対象のJointのあればAnimationがあれば値の適応を行う。下記のif文はC++17から可能になった初期化つきif文
		if (auto it = data[playingAnimationName_].node.find(joint.name); it != data[playingAnimationName_].node.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.localTF.translation = CalculateValue(rootNodeAnimation.translate.keyframes, seconds);
			joint.localTF.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, seconds).Normalize();
			joint.localTF.scale = CalculateValue(rootNodeAnimation.scale.keyframes, seconds);
		}
	}
}

bool Animation::GetPlaying() {
	// ループする場合
	if (loopFlag_) {
		// 再生しているアニメーションの名前があれば再生中
		return playingAnimationName_ != "";
	}
	// ループしない場合
	else {
		// 1未満なら再生中
		if (!reverseFlag_) { return time_ < 1.0f; }
		// 0以上なら再生中
		else { return time_ > 0.0f; }
	}
}
bool Animation::GetPlaying(const std::string& animName) {
	// そもそも再生しているアニメーションが一致しなければfalse
	if (!(playingAnimationName_ == animName)) {
		return false;
	}
	
	// ループしない場合のみ追加処理
	if(!loopFlag_) {
		// 1未満なら再生中
		if (!reverseFlag_) {
			// 再生しているアニメーションの名前が一致し、1未満なら再生中
			return time_ < 1.0f;
		}
		// 0以上なら再生中
		else {
			return time_ > 0.0f;
		}
	}

	return true;
}

void Animation::DebugGUI() {
	// アニメーション一覧
	static int currentItem = 0;
	if (!data.empty()) {
		std::vector<const char*> itemText;
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			itemText.push_back(itr->first.c_str());
		}
		ImGui::ListBox("AnimationList", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		if (ImGui::Button("Play")) { Play(itemText[currentItem]); }
		if (ImGui::Button("Play (Loop)")) { Play(itemText[currentItem], true); }
	}
	if (ImGui::TreeNode("Node")) {
		for (Joint& joint : modelPtr_->skeleton.joints) {
			if (ImGui::TreeNode(joint.name.c_str())) {
				joint.localTF.DebugGUI();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Text("--- Parameter ---");
	ImGui::SliderFloat("time", &time_, 0.0f, 1.0f);
	ImGui::Checkbox("DeltaTimeMultiply", &useDeltaTimeMultiply);
	ImGui::Checkbox("LoopFlag", &loopFlag_);
	ImGui::Checkbox("ReverseFlag", &reverseFlag_);
	ImGui::Checkbox("isActive", &isActive);
}


void Animation::LoadShortPath(const std::string& filePath, Resource::SkinningModel* ptr) {
	return LoadFullPath(kDirectoryPath + filePath, ptr);
}
void Animation::LoadFullPath(const std::string& filePath, Resource::SkinningModel* ptr) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mAnimations != 0); // アニメーションがない

	// 全アニメーションを読み取る
	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* animationAssimp = scene->mAnimations[i];	// 最初のアニメーションだけ採用。もちろん複数対応することに超したことはない
		std::string animationName = scene->mAnimations[i]->mName.C_Str();	// 最初のアニメーションだけ採用。もちろん複数対応することに超したことはない
		data[animationName].totalTime = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);	// 時間の単位を秒に変換

		// assimpでは個々のNodeのAnimationをchannelと呼んでいるので、cahnnelを回してNodeAnimationの情報を取ってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = data[animationName].node[nodeAnimationAssimp->mNodeName.C_Str()];

			// position
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				nodeAnimation.translate.keyframes.push_back({
					static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),	// ここも秒に変換
					{-keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z}	// 右手->左手
					});
			}
			// rotate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				nodeAnimation.rotate.keyframes.push_back({
					static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),	// ここも秒に変換
					{keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w}	// 右手->左手
					});
			}
			// scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				nodeAnimation.scale.keyframes.push_back({
					static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),	// ここも秒に変換
					{keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z}	// そのまま	
					});
			}
		}
	}

	// 追従セット
	modelPtr_ = ptr;
	loadedPath = filePath;
}

Vector3 Animation::CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time) {
	assert(!keyframes.empty());	// キーがないものは返す値がわからないのでダメ
	
	// キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; index++) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補完する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Utility::Interp::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time) {
	assert(!keyframes.empty());	// キーがないものは返す値がわからないのでダメ

	// キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; index++) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補完する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Utility::Interp::SlerpQuaternion(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}


const std::string Animation::kDirectoryPath = "resources/animation/";