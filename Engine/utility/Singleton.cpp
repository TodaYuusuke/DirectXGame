#include "Singleton.h"

using namespace LWP::Utility;

void Singleton::DestroyAll() {
	// 後ろから順に解放する
	while(!Singleton::ptrs_.empty()) {
		delete Singleton::ptrs_.back();
	}
	// ptrs_.clear();	// デストラクタで自動的に消されるはずなので無し
}

// 変数の実態
std::list<Singleton::DeleteObj*> Singleton::ptrs_;