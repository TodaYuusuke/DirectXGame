#include "IndexManager.h"
#include "base/ImGuiManager.h"

using namespace LWP::Utility;

IndexManager::IndexManager(int size) : kSize(size) {
	// offsetは0
	offset_ = 0;
	// サイズを決定しすべて未使用（false）で初期化
	array.resize(kSize, false);
};

IndexManager::IndexManager(int size, int offset) : kSize(size) {
	// offsetを保持
	offset_ = offset;
	// サイズを決定しすべて未使用（false）で初期化
	array.resize(kSize, false);
};

bool IndexManager::CheckEmpty() {
	for (int i = 0; i < kSize; i++) {
		// 未使用のものがあればtrueを返す
		if (!array[i]) {
			return true;
		}
	}

	// 無かったのでfalseを返す
	return false;
}
		
Index IndexManager::UseEmpty() {
	for (int i = 0; i < kSize; i++) {
		// 未使用のものがあれば処理を行う
		if (!array[i]) {
			// 使用済みに
			array[i] = true;
			// 自動解放機能付きのIndexクラスを返す
			return Index(
				[&]() { UnUse(i); },
				i + offset_
			);
		}
	}

	// 使用できなかったのでエラー
	assert(false);
	return Index(nullptr, -1);
}

void IndexManager::DebugGUI() {
	for (int i = 0; i < kSize; i++) {
		ImGui::Text("%d", static_cast<int>(array[i]));

		// 10行ごとの場合改行
		if (((i + 1) % 10 == 0)) {
			continue;
		}
		// 5列目にハイフン
		else if ((i + 1) % 5 == 0) {
			ImGui::SameLine();
			ImGui::Text("-");
		}

		// 改行キャンセル
		ImGui::SameLine();
	}
}

void IndexManager::UnUse(int index) {
	try {
		// 例外が発生する可能性のあるコード
		array.at(index) = false;
		// 例外が発生した場合、このブロック内で捕捉されます
	}
	catch (...) {
		// 何もしないで例外をスルーする
	}
}