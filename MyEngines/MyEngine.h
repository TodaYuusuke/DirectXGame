//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第１号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

#pragma once

#include "WinApp.h"
#include "DirectX.h"

#include <cassert>

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include "../math/Vector4.h"
#include "../math/Vector3.h"


class MyEngine
{	
public: // メンバ関数
	 
	/// <summary>
	/// ライブラリの初期化
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="width">ウィンドウの横幅</param>
	/// <param name="height">ウィンドウの縦幅</param>
	static void Initialize(const char* title, int width, int height);
	/// <summary>
	/// ウィンドウからのイベントをチェックする関数
	/// </summary>
	/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
	static bool ProcessMessage();


	/// <summary>
	/// フレーム開始
	/// </summary>
	static void BeginFrame();
	/// <summary>
	/// フレーム終了
	/// </summary>
	static void EndFrame();


	/// <summary>
	/// ライブラリの終了
	/// </summary>
	static void Finalize();



#pragma region 描画関数

	// 三角形描画関数
	static void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);

#pragma endregion


#pragma region その他関数
	//ーーーーーーーーーーーーーー//
	//　　　　その他の関数　　　　//
	//ーーーーーーーーーーーーーー//
private:

	// シェーダーのコンパイル関数
	static IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

#pragma endregion


#pragma region メンバ変数
	//ーーーーーーーーーーーーー//
	//　　　　メンバ変数　　　　//
	//ーーーーーーーーーーーーー//
private:

	// ウィンドウ
	static WinApp* winApp_;
	// DirectX
	static DirectX* directX_;

	// 実際に頂点リソースを作る
	static ID3D12Resource* vertexResource;
	// グラフィックスパイプライン
	static ID3D12PipelineState* graphicsPipelineState;
	// シリアライズ
	static ID3DBlob* signatureBlob;
	static ID3DBlob* errorBlob;
	// RootSignature
	static ID3D12RootSignature* rootSignature;
	// PixelShader
	static IDxcBlob* pixelShaderBlob;
	// VertexShader
	static IDxcBlob* vertexShaderBlob;

	// ビューポート
	static D3D12_VIEWPORT viewport;
	// シザー矩形
	static D3D12_RECT scissorRect;

	// 頂点バッファビューを作成する
	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

#pragma endregion

	

#pragma region 描画変数

	// 三角形の最大数
	static const int32_t kMaxTriangleCount = 32768;
	// 三角形の頂点数
	static const UINT kVertexCountTriangle = 3;

	// 三角形の描画数
	static int triangleCount;
	// 頂点リソース
	static Vector4* vertexData;

#pragma endregion

};