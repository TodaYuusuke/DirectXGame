#pragma once
#include "DirectXCommon.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC) && !defined(NEW)
#define NEW  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW  new
#endif

class DrawSystem
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DrawSystem* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* DirectXCommon);

	// 描画関数たち
	void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);

private: // メンバ変数
	// DirectXCommon管理
	DirectXCommon* DirectXCommon_ = nullptr;

	// 構造体宣言
	struct DXC {
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	};
	struct PipelineSet {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	};
	
	struct VectorPosColor {
		Vector4 position;
		Vector3 color;
	};
	struct VertexTriangle {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
		// 三角形の頂点リソース
		VectorPosColor* vertexData_ = nullptr;
		// 三角形の描画数
		uint32_t triangleCount_ = 0;
	};

	// 描画関連
	std::unique_ptr<DXC> dxc_;
	std::unique_ptr<PipelineSet> pipelineSet_;
	std::unique_ptr<VertexTriangle> vertexTriangle_;

	// 三角形の最大数
	static const int32_t kMaxTriangleCount_ = 32768;
	// 三角形の頂点数
	static const UINT kVertexCountTriangle_ = 3;

private: // 非公開のメンバ関数
	DrawSystem() = default;
	~DrawSystem() = default;
	DrawSystem(const DrawSystem&) = delete;
	const DrawSystem& operator=(const DrawSystem&) = delete;

	/// <summary>
	/// DXC初期化
	/// </summary>
	void InitialzieDXC();

#pragma region PSO生成関連

	void CreateRootSignature();
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_BLEND_DESC CreateBlendState();
	D3D12_RASTERIZER_DESC CreateRasterizerState();
	IDxcBlob* CreateVertexShader();
	IDxcBlob* CreatePixelShader();

#pragma endregion

	/// <summary>
	/// グラフィックスパイプラインを作成
	/// </summary>
	void CreateGraphicsPipeLineState();

#pragma region 頂点バッファ

	/// <summary>
	/// VertexResourceを作成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// 頂点バッファビューを作成
	/// </summary>
	void CreateVertexBufferView();

#pragma endregion

	/// <summary>
	/// 三角形の頂点バッファを作成
	/// </summary>
	void CreateVerTexTriangle();

	/// <summary>
	/// シェーダーのコンパイル関数
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);
};

