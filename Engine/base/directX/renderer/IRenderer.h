#pragma once
#include "ResourceStruct.h"
#include "base/directX/utility/RootSignature.h"
#include "base/directX/utility/PSO.h"
#include "base/directX/utility/resource/data/StructuredBuffer.h"

namespace LWP::Base {
	/// <summary>
	/// リソースに書き込むレンダラーの基底クラス
	/// </summary>
	class IRenderer {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IRenderer() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~IRenderer() = default;


	protected: // ** メンバ変数 ** //

		// Viewをセットする関数ポインタ
		std::function<void()> setViewFunction_;
	};
}