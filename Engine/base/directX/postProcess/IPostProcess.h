#pragma once
#include <string>
#include <fstream>

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ポストプロセス達の基底クラス
	/// </summary>
	class IPostProcess {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		IPostProcess() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPostProcess() = default;

		// シェーダーでincludeする処理を書き込む
		virtual void WriteInclude(std::ofstream* stream) = 0;
		// シェーダー内の処理を書き込む
		virtual void WriteFunction(std::ofstream* stream) = 0;

		
	public: // ** 共通のメンバ変数 ** //

		// 使用するかフラグ
		bool use = false;
	};
}