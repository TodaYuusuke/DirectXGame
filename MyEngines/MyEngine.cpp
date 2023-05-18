#include "MyEngine.h"
#include "MyUtility.h"
#include <format>

/// <summary>
/// ライブラリの初期化
/// </summary>
/// <param name="title">ウィンドウタイトル</param>
/// <param name="width">ウィンドウの横幅</param>
/// <param name="height">ウィンドウの縦幅</param>
void MyEngine::Initialize(const char* title, int width, int height) {

	// インスタンスを受け取る
	winApp_ = WinApp::GetInstance();
	directX_ = DirectX::GetInstance();

	// 初期化
	winApp_->Initialize(title, width, height);
	directX_->Initialize(winApp_, width, height);

#pragma region DXCの初期化

	// dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region PSO生成

#pragma region RootSignature

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// シリアライズしてバイナリにする
	signatureBlob = nullptr;
	errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		MyUtility::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	rootSignature = nullptr;
	hr = directX_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

#pragma endregion
#pragma region InputLayout

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion
#pragma region BlendState

	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma endregion
#pragma region RasterizerState

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion
#pragma region VertexShader

	// シェーダーをコンパイルする
	vertexShaderBlob = CompileShader(L"./Resources/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

#pragma endregion
#pragma region PixelShader

	// シェーダーをコンパイルする
	pixelShaderBlob = CompileShader(L"./Resources/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

#pragma endregion

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;	// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;	// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };	// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;			// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;	// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	graphicsPipelineState = nullptr;
	hr = directX_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region VertexResourceを生成

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * kMaxTriangleCount * kVertexCountTriangle;; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	vertexResource = nullptr;
	hr = directX_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

#pragma endregion


#pragma region VertexBufferViewを作成

	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * kMaxTriangleCount * kVertexCountTriangle;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	// 頂点リソースにデータを書き込む
	vertexData = nullptr;

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
#pragma endregion
}

/// <summary>
/// ウィンドウからのイベントをチェックする関数
/// </summary>
/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
bool MyEngine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}


/// <summary>
/// フレーム開始
/// </summary>
void MyEngine::BeginFrame() {
	directX_->PreDraw();
}
/// <summary>
/// フレーム終了
/// </summary>
void MyEngine::EndFrame() {
	directX_->PostDraw();
}


/// <summary>
/// ライブラリの終了
/// </summary>
void MyEngine::Finalize() {

	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	CloseWindow(winApp_->GetHWND());

	//// リソースリークチェック
	//IDXGIDebug1* debug;
	//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
	//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	//	debug->Release();
	//}
}



#pragma region 描画関数

void MyEngine::DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color) {

	// 最大数を超えていないかチェック
	assert(triangleCount < kMaxTriangleCount);

	// Resourceにデータを書き込む 
	int index = triangleCount * kVertexCountTriangle;

	vertexData[index] = { pos1.x,pos1.y,pos1.z,1.0f };
	vertexData[index + 1] = { pos2.x,pos2.y,pos2.z,1.0f };
	vertexData[index + 2] = { pos3.x,pos3.y,pos3.z,1.0f };


	// コマンドを積む
	ID3D12GraphicsCommandList* commandList = directX_->GetCommandList();
	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->SetPipelineState(graphicsPipelineState);		// PSOを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);	// VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス、インスタンスについては今後
	commandList->DrawInstanced(3, 1, triangleCount * 3, 0);

	color;
	// 描画数+1
	triangleCount++;
}

#pragma endregion



#pragma region その他関数
//ーーーーーーーーーーーーーー//
//　　　　その他の関数　　　　//
//ーーーーーーーーーーーーーー//


// シェーダーのコンパイル関数
IDxcBlob* MyEngine::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

	/*-- 1.hlslファイルを読む --*/

	// これからシェーダーをコンパイルする旨をログに出す
	MyUtility::Log(MyUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読まなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	/*-- 2.Compileする --*/

	LPCWSTR arguments[] = {
		filePath.c_str(),			// コンパイル対象のhlslファイル名
		L"-E",L"main",				// エントリーポイントの指定、基本的にmain以外にはしない
		L"-T",profile,				// ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		// 読み込んだファイル
		arguments,					// コンパイルオプション
		_countof(arguments),		// コンパイルオプションの数
		includeHandler,				// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));


	/*-- 3.警告・エラーがでていないか確認する --*/

	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		MyUtility::Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}


	/*-- 4.Compile結果を受け取って返す --*/

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	MyUtility::Log(MyUtility::ConvertString(std::format(L"CompileSucceeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}
#pragma endregion


#pragma region メンバ変数
//ーーーーーーーーーーーーー//
//　　　　メンバ変数　　　　//
//ーーーーーーーーーーーーー//


WinApp* MyEngine::winApp_ = nullptr;
DirectX* MyEngine::directX_ = nullptr;

// 実際に頂点リソースを作る
ID3D12Resource* MyEngine::vertexResource;
// グラフィックスパイプライン
ID3D12PipelineState* MyEngine::graphicsPipelineState;
// シリアライズ
ID3DBlob* MyEngine::signatureBlob;
ID3DBlob* MyEngine::errorBlob;
// RootSignature
ID3D12RootSignature* MyEngine::rootSignature;
// PixelShader
IDxcBlob* MyEngine::pixelShaderBlob;
// VertexShader
IDxcBlob* MyEngine::vertexShaderBlob;

// ビューポート
D3D12_VIEWPORT MyEngine::viewport;
// シザー矩形
D3D12_RECT MyEngine::scissorRect;

// 頂点バッファビューを作成する
D3D12_VERTEX_BUFFER_VIEW MyEngine::vertexBufferView;



#pragma region 描画変数

// 三角形の描画数
int MyEngine::triangleCount;
// 頂点リソース
Vector4* MyEngine::vertexData;

#pragma endregion

#pragma endregion
