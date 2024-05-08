#pragma once
#include "../IPostProcess.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// グレースケール
	/// </summary>
	class GrayScale final
		: public IPostProcess {
	public: // ** パブリックなメンバ変数 ** //

		// なし！
	
	public: // ** メンバ関数 ** //

		// シェーダーでincludeする処理を書き込む
		void WriteInclude(std::ofstream* stream) {
			*stream << R"(
"#include "GrayScale.hlsli"
)";
		};
		// シェーダー内の処理を書き込む
		void WriteFunction(std::ofstream* stream) {
			*stream << R"(
	output.rgb = GrayScale(output.rgb);
)";
		};

	};
}