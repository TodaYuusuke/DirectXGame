#include "AudioTest.h"
#include "LevelLoaderTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

void AudioTest::Initialize() {
	audio.LoadShortPath("powerup01.mp3");
}
void AudioTest::Update() {
	ImGui::Begin("Test");
	audio.DebugGUI();
	ImGui::End();
}