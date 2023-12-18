#pragma once
#include "../../../Externals/DirectXTex/DirectXTex.h"
#include "../../../Engine/object/WorldTransform.h"
#include <string>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace LWP::Base {
	class DirectXCommon;
	class CommandManager;
}

namespace LWP::Resource {
	class Texture final {
	public:
		// ** 共通の変数 ** //

		// コンストラクタ
		Texture(Base::CommandManager* manager, const std::string& filePath);
		// デストラクタ
		~Texture() = default;

		// サイズをVector2で受け取る
		LWP::Math::Vector2 GetTextureSize() const;

	private: // ** プロパティ変数 ** //
		// CommandManager上でのインデックス
		int index_;
	public: // アクセッサ
		// CommandManager上でのインデックスを読みとる
		int GetIndex() const { return index_; }
		// 別のMaterialと同期する
		void SetIndex(int value) { index_ = value; }


	private: // ** メンバ変数 ** //

		// ミップマップ付きテクスチャ
		DirectX::ScratchImage mipImages;


	private: // ** プライベートな関数 ** //
		/// <summary>
		/// テクスチャを読み込む
		/// </summary>
		void Load(const std::string& filePath);
	};
}