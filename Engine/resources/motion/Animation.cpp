#include "Animation.h"

#include "component/Resource.h"
#include "component/Information.h"
#include "utility/motionEffects/Interpolation.h"

#include "primitive/Node.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Math;

Animation::Animation() {
	// インスタンスをエンジンに送る
	SetInstance(this);
}
Animation::~Animation() {
	// ここでどうにかしてエンジン上のポインタを破棄させたい！
	DeleteInstance(this);
}

void Animation::Start() { Start(0.0f); }
void Animation::Start(float startSec) {
	isStart_ = true;
	time_ = startSec;
}
void Animation::Stop() { isStart_ = false; }

void Animation::Update() {
	if (!isStart_) { return; }	// 早期リターン

	// 時間を更新
	time_ += useDeltaTimeMultiply_ ? Info::GetDeltaTimeF() : Info::GetDefaultDeltaTimeF();
	// 最後までいったらリピート再生。リピートしなくても別に良い
	time_ = std::fmod(time_, duration_);

	// LocalMatrixを生成し、Nodeに適応（現在はrootNodeのみ適応）
	Primitive::Node& node = meshPtr_->node;
	
	// Nodeのアニメーションを取得
	NodeAnimation& nodeAnimation = nodeAnimations[node.name];
	node.localMatrix = Matrix4x4::CreateAffineMatrix(
		CalculateValue(nodeAnimation.scale.keyframes, time_),
		//Vector3{0.0f,0.0f,0.0f},
		CalculateValue(nodeAnimation.rotate.keyframes, time_),
		CalculateValue(nodeAnimation.translate.keyframes, time_)
	);

}

bool Animation::isEnd() { return !isStart_; }

void Animation::LoadAnimation(std::string filepath, Primitive::Mesh* ptr) {
	return LoadAnimationLongPath(kDirectoryPath + filepath, ptr);
}
void Animation::LoadAnimationLongPath(std::string filepath, Primitive::Mesh* ptr) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), 0);
	assert(scene->mAnimations != 0); // アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];	// 最初のアニメーションだけ採用。もちろん複数対応することに超したことはない
	duration_ = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);	// 時間の単位を秒に変換

	// assimpでは個々のNodeのAnimationをchannelと呼んでいるので、cahnnelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		// position
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			nodeAnimation.translate.keyframes.push_back({
				static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),	// ここも秒に変換
				{-keyAssimp.mValue.x, keyAssimp.mValue.y, -keyAssimp.mValue.z}	// 右手->左手
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

	// 追従セット
	meshPtr_ = ptr;
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