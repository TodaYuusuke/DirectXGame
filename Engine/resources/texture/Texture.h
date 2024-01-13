#pragma once
#include "ITexture.h"
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
	class Texture final
		: public ITexture {
	public:
		// ** 共通の変数 ** //

		// コンストラクタ
		Texture(Base::CommandManager* manager, const std::string& filePath);
		// デストラクタ
		~Texture() = default;

		// サイズをVector2で受け取る
		LWP::Math::Vector2 GetTextureSize() const override;

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