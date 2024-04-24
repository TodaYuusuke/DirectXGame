#include "Audio.h"

#include "component/Resource.h"

using namespace LWP;
using namespace LWP::Resource;

void Audio::Load(const std::string& path) {
	ptr_ = LoadAudio(path);
}
void Audio::LoadLongPath(const std::string& path) {
	ptr_ = LoadAudioLongPath(path);
}

void Audio::Play(const UINT& loopCount) {
	ptr_->Play(volume_, loopCount);
}
void Audio::Play(const float& volume, const UINT& loopCount) {
	volume_ = volume;
	ptr_->Play(volume_, loopCount);
}
void Audio::Stop() {
	ptr_->Stop();
}

float Audio::GetVolume() {
	return volume_;
}
void Audio::SetVolume(float value) {
	volume_ = value;
	ptr_->SetVolume(value);
}