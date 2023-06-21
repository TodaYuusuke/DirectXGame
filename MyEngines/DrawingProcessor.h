#pragma once
#include "DirectXCommon.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>

class DrawingProcessor
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DrawingProcessor* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* DirectXCommon);

	// 描画関数たち
	void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);

private: // メンバ変数
	// DirectXCommon管理
	DirectXCommon* directXCommon_ = nullptr;

	// 構造体宣言

	//*** DirectXシェーダコンパイラ ***//
	// HLSLコードをバイナリ形式のGPUシェーダーに変換する
	struct DXC {
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;				// dxcの汎用オブジェクト
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;			// dxcのコンパイラオブジェクト
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;	// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
	};
	struct PipelineSet {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;			// リソースとシェーダーのバインディングを定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;	// グラフィックパイプラインの状態を定義
	};
	
	struct VectorPosColor {
		Vector4 position;
		Vector4 color;
	};
	struct VertexTriangle {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;		// GPU上の頂点データの格納場所
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;	// マテリアルデータの格納場所
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};				// BufferLocationは頂点データ格納場所のアドレス
		// 三角形の頂点リソース
		VectorPosColor* vertexData_ = nullptr;
		// 三角形の描画数
		uint32_t triangleCount_ = 0;
	};

	// 描画関連
	std::unique_ptr<DXC> dxc_;
	std::unique_ptr<PipelineSet> pipelineSet_;
	// 三角形
	std::unique_ptr<VertexTriangle> vertexTriangle_;

	// 三角形の最大数
	static const int32_t kMaxTriangleCount_ = 32768;
	// 三角形の頂点数
	static const UINT kVertexCountTriangle_ = 3;

private: // 非公開のメンバ関数
	DrawingProcessor() = default;
	~DrawingProcessor() = default;
	DrawingProcessor(const DrawingProcessor&) = delete;
	const DrawingProcessor& operator=(const DrawingProcessor&) = delete;

	/// <summary>
	/// DXC初期化
	/// </summary>
	void InitialzieDXC();

#pragma region PipelineSet

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

#pragma endregion



	/// <summary>
	/// 頂点バッファビューを作成
	/// </summary>
	void CreateVertexTriangleBufferView();


	/// <summary>
	/// 三角形の頂点バッファを作成
	/// </summary>
	void CreateVerTexTriangle();


	/// <summary>
	/// VertexResourceを作成
	/// </summary>
	ID3D12Resource* CreateBufferResource(size_t);

	/// <summary>
	/// シェーダーのコンパイル関数
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	/// <summary>
	/// unsinged int型の色情報をVector4に変換する
	/// </summary>
	/// <returns>Vector4型の色情報</returns>
	Vector4 HexColorToVector4(unsigned int);
};

