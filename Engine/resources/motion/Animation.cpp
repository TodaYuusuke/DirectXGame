#include "Animation.h"

#include "component/Resource.h"
#include "component/Information.h"
#include "utility/motionEffects/Interpolation.h"

#include "resources/model/SkinningModel.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Primitive;
using namespace LWP::Utility;
using namespace LWP::Math;

Animation::Animation() {
	// インスタンスをエンジンに送る
	Resource::Manager::GetInstance()->SetPointer(this);
}
Animation::~Animation() {
	// エンジン上のポインタを破棄
	Resource::Manager::GetInstance()->DeletePointer(this);
}

void Animation::Init() {
	// 全トラック初期化
	for (int i = 0; i < int(TrackType::Count); i++) {
		tracks_[TrackType(i)].Init();
	}
}

Animation& Animation::Play(const std::string animName, float transitionTime, float startTime, TrackType type) {
	// 存在するアニメーションかチェック
	assert(data.contains(animName) && "animName is Not Found.");
	// トラックタイプが正常かチェック
	assert((TrackType::Main <= type && type < TrackType::Count) && "TrackType is Invalid.");

	State& s = tracks_[type];
	s.playingAnimationName = animName;
	s.time = startTime;
	s.totalSeconds = data.at(animName).totalTime;
	s.isPause = false;

	// トランジション時間が0.0f以上ならば、トランジションを行う
	transition_.totalTime = transitionTime;
	if (transition_.totalTime > 0.0f) {
		transition_.isActive = true;
		transition_.t = 0.0f;	// トランジションの経過割合を初期化
		// スケルトンをコピーしておく
		transition_.tempSkeleton.joints.resize(modelPtr_->skeleton.joints.size());
		std::copy(modelPtr_->skeleton.joints.begin(), modelPtr_->skeleton.joints.end(), transition_.tempSkeleton.joints.begin());
	}

	return *this;
}
Animation& Animation::Pause(TrackType type)	{ tracks_[type].isPause = true;		return *this; }
Animation& Animation::Resume(TrackType type){ tracks_[type].isPause = false;	return *this; }
Animation& Animation::Stop(TrackType type)	{
	tracks_[type].isPause = true;
	tracks_[type].playingAnimationName = "";
	if (type == TrackType::Blend) {
		// ブレンドを停止した場合は再生速度を元に戻す
		tracks_[TrackType::Main].playbackSpeed = 1.0f;	// メインの再生速度を元に戻す
		tracks_[TrackType::Blend].playbackSpeed = 1.0f;	// ブレンドの再生速度もついでに戻す
	}
	return *this;
}

Animation& Animation::Loop(TrackType type)					{ tracks_[type].isLoop = !tracks_[type].isLoop;					return *this; }
Animation& Animation::Loop(bool b, TrackType type)			{ tracks_[type].isLoop = b;										return *this; }
Animation& Animation::Reverse(TrackType type)				{ tracks_[type].isReverse = !tracks_[type].isReverse;			return *this; }
Animation& Animation::Reverse(bool b, TrackType type)		{ tracks_[type].isReverse = b;									return *this; }
Animation& Animation::UseTimeScale(TrackType type)			{ tracks_[type].isUseTimeScale = !tracks_[type].isUseTimeScale;	return *this; }
Animation& Animation::UseTimeScale(bool b, TrackType type)	{ tracks_[type].isUseTimeScale = b;								return *this; }

void Animation::Update() {
	State& main = tracks_[TrackType::Main];
	State& blend = tracks_[TrackType::Blend];

	// メインとブレンドどちらもアニメーションを再生しているなら
	if (main.GetPlaying() && blend.GetPlaying()) {
		// 綺麗に同期してブレンドするためにアニメーションに合わせて速度スケールを計算
		main.playbackSpeed = Interp::LerpF(1.0f, main.totalSeconds / blend.totalSeconds, blendT);
		blend.playbackSpeed = Interp::LerpF(1.0f, blend.totalSeconds / main.totalSeconds, 1.0f - blendT);
	}
	// 更新
	main.Update();
	blend.Update();
	// 同時再生更新
	for (int i = int(TrackType::Multi0); i < int(TrackType::Count); i++) {
		tracks_[TrackType(i)].Update();
	}


	// Joint更新
	UpdateJoint();
}

void Animation::Sync() {
	tracks_[TrackType::Blend].time = tracks_[TrackType::Main].time;
}
void Animation::DebugGUI() {
	static int currentItem = 0;
	std::vector<const char*> itemText;
	if (!data.empty()) {
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			itemText.push_back(itr->first.c_str());
		}
	}

	// アニメーション一覧
	ImGui::ListBox("AnimationList", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
	// 推移に掛かる時間の設定
	static float transitionTime = 0.0f;
	ImGui::DragFloat("Transition Time", &transitionTime, 0.01f);
	ImGui::Text("-----------------");

	// 変更されたかのフラグ
	bool isChanged = false;
	// 各トラックごとのGUI表示用関数
	auto trackGUI = [this, itemText, &isChanged](TrackType type, std::string uniqueTag) {
		ImGui::Text(("[ " + uniqueTag + " ]").c_str());
		ImGui::Text("Playing Animation : %s", tracks_[type].playingAnimationName.c_str());
		ImGui::Text("TotalTime %f", tracks_[type].totalSeconds);
		if (ImGui::Button("Play")) { Play(itemText[currentItem], transitionTime, {}, type); }
		ImGui::SameLine();
		if (ImGui::Button("Pause")) { Pause(type); }
		ImGui::SameLine();
		if (ImGui::Button("Resume")) { Resume(type); }
		ImGui::SameLine();
		if (ImGui::Button("Stop")) { Stop(type); }
		isChanged |= tracks_[type].DebugGUI();
		ImGui::EndTabItem();
	};

	int kTrackCount = int(TrackType::Count);
	if (ImGui::BeginTabBar("Tracks", kTrackCount)) {
		if (ImGui::BeginTabItem("Main")) {
			trackGUI(TrackType::Main, "Main");
		}
		if (ImGui::BeginTabItem("Blend")) {
			trackGUI(TrackType::Blend, "Blend");
		}
		if (ImGui::BeginTabItem("Multi0")) {
			trackGUI(TrackType::Multi0, "Multi0");
		}
		if (ImGui::BeginTabItem("Multi1")) {
			trackGUI(TrackType::Multi1, "Multi1");
		}
		if (ImGui::BeginTabItem("Multi2")) {
			trackGUI(TrackType::Multi2, "Multi2");
		}
		if (ImGui::BeginTabItem("Multi3")) {
			trackGUI(TrackType::Multi3, "Multi3");
		}
		if (ImGui::BeginTabItem("Multi4")) {
			trackGUI(TrackType::Multi4, "Multi4");
		}
		ImGui::EndTabBar();
	}

	ImGui::Text("-----------------");
	ImGui::SliderFloat("Transition T", &transition_.t, 0.0f, 1.0f);
	ImGui::SliderFloat("Blend T", &blendT, 0.0f, 1.0f);
	if (ImGui::Button("Sync main & blend")) { Sync(); }

	if (modelPtr_ && ImGui::TreeNode("Model")) {
		modelPtr_->DebugGUI();
		ImGui::TreePop();
	}

	if (isChanged) { UpdateJoint(); }
}

void Animation::LoadShortPath(const std::string& filePath, Resource::SkinningModel* ptr) {
	return LoadFullPath(kDirectoryPath_ + filePath, ptr);
}
void Animation::LoadFullPath(const std::string& filePath, Resource::SkinningModel* ptr) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mAnimations != 0); // アニメーションがない

	// 全アニメーションを読み取る
	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* animationAssimp = scene->mAnimations[i];	// アニメーションを１つ取り出す
		std::string animationName = scene->mAnimations[i]->mName.C_Str();	// アニメーション名を取得
		data[animationName].totalTime = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);	// 時間の単位を秒に変換

		// assimpでは個々のNodeのAnimationをchannelと呼んでいるので、cahnnelを回してNodeAnimationの情報を取ってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = data.at(animationName).node[nodeAnimationAssimp->mNodeName.C_Str()];

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
	transition_.tempSkeleton = modelPtr_->skeleton;	// スケルトンのコピーを作成しておく
	loadedPath = filePath;
}

std::vector<std::string> Animation::GetAnimationNames() const {
	std::vector<std::string> names;
	for (const auto& pair : data) {
		names.push_back(pair.first);
	}
	return names;
}
float Animation::GetTotalSeconds(const std::string& animName) {
	// 存在するアニメーションかチェック
	assert(data.contains(animName) && "animName is Not Found.");
	return data.at(animName).totalTime;
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
			return Interp::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
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
			return Interp::SlerpQuaternion(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

void Animation::UpdateJoint() {
	State& mainState = tracks_[TrackType::Main];
	State& blendState = tracks_[TrackType::Blend];
	bool mainF = mainState.GetPlaying();
	bool blendF = blendState.GetPlaying();


	// 再生中のアニメーションがないならば
	if (!mainF && !blendF) { return; }	// 早期リターン

	// アニメーションの時間
	float mainSeconds = mainState.totalSeconds * mainState.time;
	float blendSeconds = blendState.totalSeconds * blendState.time;
	// トランジション用のtを求める
	transition_.t += mainState.GetDeltaTime() / transition_.totalTime;	// いったんメインと同じデルタタイムを使う
	// 1を超えないように
	if (transition_.t > 1.0f) { transition_.t = 1.0f; }

	for (Joint& joint : modelPtr_->skeleton.joints) {
		Object::TransformQuat mainTF = joint.localTF;
		Object::TransformQuat blendTF = mainTF;

		// メイン用のAnimationがあれば値の適応を行う。
		if (mainF) {
			AnimationData& mainData = data.at(mainState.playingAnimationName);
			// 対象のJointのAnimationがあれば値の適応を行う。下記のif文はC++17から可能になった初期化つきif文
			if (auto it = mainData.node.find(joint.name); it != mainData.node.end()) {
				const NodeAnimation& rootNodeAnimation = (*it).second;
				mainTF.translation = CalculateValue(rootNodeAnimation.translate.keyframes, mainSeconds);
				mainTF.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, mainSeconds).Normalize();
				mainTF.scale = CalculateValue(rootNodeAnimation.scale.keyframes, mainSeconds);
			}
		}
		// ブレンド用のAnimationがあれば値の適応を行う。
		if (blendF) {
			AnimationData& blendData = data.at(blendState.playingAnimationName);
			// 同じく対象のJointのAnimationがあれば値の適応を行う。
			if (auto it = blendData.node.find(joint.name); it != blendData.node.end()) {
				const NodeAnimation& rootNodeAnimation = (*it).second;
				blendTF.translation = CalculateValue(rootNodeAnimation.translate.keyframes, blendSeconds);
				blendTF.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, blendSeconds).Normalize();
				blendTF.scale = CalculateValue(rootNodeAnimation.scale.keyframes, blendSeconds);
			}
		}

		// メインとブレンドどちらもアニメーションを再生しているなら
		if (mainF && blendF) {
			joint.localTF.translation = Interp::Lerp(mainTF.translation, blendTF.translation, blendT);
			joint.localTF.rotation = Interp::SlerpQuaternion(mainTF.rotation, blendTF.rotation, blendT);
			joint.localTF.scale = Interp::Lerp(mainTF.scale, blendTF.scale, blendT);
		}
		// メインだけアニメーションを再生しているなら
		else if (mainF) {
			joint.localTF.translation = mainTF.translation;
			joint.localTF.rotation = mainTF.rotation;
			joint.localTF.scale = mainTF.scale;
		}
		// ブレンドだけアニメーションを再生しているなら
		else {
			joint.localTF.translation = blendTF.translation;
			joint.localTF.rotation = blendTF.rotation;
			joint.localTF.scale = blendTF.scale;
		}

		// 同時再生系の処理を行う
		for (int i = int(TrackType::Multi0); i < int(TrackType::Count); i++) {
			State& multiState = tracks_[TrackType(i)];
			// 再生中のアニメーションがあるならば
			if (multiState.GetPlaying()) {
				float multiSeconds = multiState.totalSeconds * multiState.time;
				AnimationData& multiData = data.at(multiState.playingAnimationName);
				// 対象のJointのAnimationがあれば値の適応を行う。下記のif文はC++17から可能になった初期化つきif文
				if (auto it = multiData.node.find(joint.name); it != multiData.node.end()) {
					const NodeAnimation& rootNodeAnimation = (*it).second;
					Object::TransformQuat multiTF;
					multiTF.translation = CalculateValue(rootNodeAnimation.translate.keyframes, multiSeconds);
					multiTF.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, multiSeconds).Normalize();
					multiTF.scale = CalculateValue(rootNodeAnimation.scale.keyframes, multiSeconds);

					// そのジョイントの情報を差し替える
					joint.localTF.translation = multiTF.translation;
					joint.localTF.rotation = multiTF.rotation;
					joint.localTF.scale = multiTF.scale;
				}
			}
		}

		// もしトランジションが有効ならば、処理を行う
		if (transition_.isActive) {
			// 補完元のスケルトンのトランスフォームを取得	
			Object::TransformQuat prevTransform = transition_.tempSkeleton.joints[transition_.tempSkeleton.jointMap[joint.name]].localTF;
			// 線形補間によるアニメーションの補完を行う
			joint.localTF.translation = Interp::Lerp(prevTransform.translation, joint.localTF.translation, transition_.t);
			joint.localTF.rotation = Interp::SlerpQuaternion(prevTransform.rotation, joint.localTF.rotation, transition_.t);
			joint.localTF.scale = Interp::Lerp(prevTransform.scale, joint.localTF.scale, transition_.t);
		}
	}


	// あとでfalseにする
	if (transition_.t >= 1.0f) {
		transition_.isActive = false;	// トランジションを終了
	}
}

void Animation::State::Update() {
	// isPause_がtrue、もしくは再生中のアニメーションが""のときは再生しない
	if (isPause || playingAnimationName == "") { return; }	// 早期リターン

	// 時間を更新
	float t = GetDeltaTime() / totalSeconds;	// こちらはデルタタイム更新
	if (!isReverse) { time += t; }	// リバースフラグに応じて進行方向を変える
	else { time -= t; }

	// ループする場合
	if (isLoop) {
		// 最後までいったらリピート再生
		if (!isReverse) { time = std::fmod(time, 1.0f); }
		// リバースするなら0を下回ったら1を足して戻す
		else { if (time < 0.0f) { time += 1.0f; } }
	}
	// ループしない場合
	else {
		// 1を超えないように
		if (!isReverse) { if (time > 1.0f) { time = 1.0f; } }
		// リバースするなら0を超えないように
		else { if (time < 0.0f) { time = 0.0f; } }
	}
}
float Animation::State::GetDeltaTime() {
	return (isUseTimeScale ? Info::GetDeltaTimeF() : Info::GetDefaultDeltaTimeF()) * playbackSpeed;
}
bool Animation::State::GetPlaying(const std::string& animName) {
	// 再生中のアニメーションがなければfalse
	if (playingAnimationName.empty()) { return false; }
	// アニメーション名が指定されている場合、それと一致しているか確認
	if ((!animName.empty() && playingAnimationName != animName)) { return false; }

	// ループ再生の場合
	if (isLoop) { return true; }
	// 非ループ再生の場合
	else {
		// 再生方向によって再生中かどうかを判定
		if (!isReverse) {
			return time < 1.0f;
		}
		else {
			return time > 0.0f;
		}
	}
}
bool Animation::State::DebugGUI() {
	float temp = time;
	ImGui::SliderFloat("time", &time, 0.0f, 1.0f);
	ImGui::DragFloat("PlaybackSpeed", &playbackSpeed, 0.01f);	// 再生速度倍率
	if (ImGui::Button("x0.5 >>")) { playbackSpeed = 0.5f; }	// 再生速度を0.5に
	ImGui::SameLine();
	if (ImGui::Button("x1.0 >>")) { playbackSpeed = 1.0f; }	// 再生速度を1.0に
	ImGui::SameLine();
	if (ImGui::Button("x1.5 >>")) { playbackSpeed = 1.5f; }	// 再生速度を1.5に
	ImGui::SameLine();
	if (ImGui::Button("x2.0 >>")) { playbackSpeed = 2.0f; }	// 再生速度を2.0に
	ImGui::Checkbox("isPause", &isPause);				// 一時停止フラグ
	ImGui::Checkbox("isLoop", &isLoop);					// ループフラグ
	ImGui::Checkbox("isReverse", &isReverse);			// 逆再生フラグ
	ImGui::Checkbox("isUseTimeScale", &isUseTimeScale);	// タイムスケールフラグ

	return temp != time;	// tの値が変わったらJointを更新するため、trueを返す
}

const std::string Animation::kDirectoryPath_ = "resources/animation/";