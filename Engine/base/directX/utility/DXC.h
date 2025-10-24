#pragma once
#include <wrl.h>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include <string>
#include "utility/other/Singleton.h"

namespace LWP::Base {
	/// <summary>
	/// DirectXシェーダコンパイラ
	/// <para>HLSLコードをバイナリ形式のGPUシェーダーに変換する</para>
	/// </summary>
	class DXC final : public Utility::ISingleton<DXC> {
		friend class Utility::ISingleton<DXC>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		DXC();


	public:	// ** メンバ関数 ** //
		
		/// <summary>
		/// シェーダーをコンパイルする関数
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="profile"></param>
		/// <returns></returns>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

		
		// dxcの汎用オブジェクト
		IDxcUtils* Utils() { return utils_.Get(); }
		// dxcのコンパイラオブジェクト
		IDxcCompiler3* Compiler() { return compiler_.Get(); }
		// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
		IDxcIncludeHandler* IncludeHandler() { return includeHandler_.Get(); }


	private: // ** メンバ変数 ** //
		
		// dxcの汎用オブジェクト
		Microsoft::WRL::ComPtr<IDxcUtils> utils_;
		// dxcのコンパイラオブジェクト
		Microsoft::WRL::ComPtr<IDxcCompiler3> compiler_;
		// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	};
}