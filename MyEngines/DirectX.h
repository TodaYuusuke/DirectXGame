#pragma once
#include <string>


class DirectX
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectX* GetInstance();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	

	/// <summary>
	/// wstring -> stringへの変換
	/// </summary>
	static std::string ConvertString(const std::wstring& str);
	/// <summary>
	/// シェーダーのコンパイル関数
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);



private: // メンバ変数


};

