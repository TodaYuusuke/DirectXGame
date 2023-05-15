//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第１号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

#pragma once
#include <Windows.h>
#include <cstdint>
#include <format>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include "math/Vector4.h"
#include "math/Vector3.h"

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

	static void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);

#pragma endregion


#pragma region その他関数
	//ーーーーーーーーーーーーーー//
	//　　　　その他の関数　　　　//
	//ーーーーーーーーーーーーーー//
private:

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT msg, WPARAM wparam, LPARAM lparam);
	// ログの表示
	static void Log(const std::string& message);

	// string -> wstringへの変換
	static std::wstring ConvertString(const std::string& str);
	// wstring -> stringへの変換
	static std::string ConvertString(const std::wstring& str);

	// シェーダーのコンパイル関数
	static IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
#pragma endregion


#pragma region メンバ変数
	//ーーーーーーーーーーーーー//
	//　　　　メンバ変数　　　　//
	//ーーーーーーーーーーーーー//
private:

	// ウィンドウ
	static HWND hwnd;

	// フェンス
	static ID3D12Fence* fence;
	static uint16_t fenceValue;
	// フェンスイベント
	static HANDLE fenceEvent;
	// コマンドキュー
	static ID3D12CommandQueue* commandQueue;
	// コマンドアロケーター
	static ID3D12CommandAllocator* commandAllocateor;
	// コマンドリスト
	static ID3D12GraphicsCommandList* commandList;
	// スワップチェーン
	static IDXGISwapChain4* swapChain;
	// ディスクリプタ
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[];
	// ディスクリプタヒープ
	static ID3D12DescriptorHeap* rtvDescriptorHeep;
	// スワップチェーンリソース
	static ID3D12Resource* swapChainResources[];
	// トランジションバリア
	static D3D12_RESOURCE_BARRIER barrier;
	// デバイス
	static ID3D12Device* device;
	// 使用するアダプタ用の変数
	static IDXGIAdapter4* useAdapter;
	// ファクトリーの生成
	static IDXGIFactory7* dxgiFactory;
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

	// DEBUG
	static ID3D12Debug1* debugController;
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