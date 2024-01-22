#include "PSO.h"
#include "../Engine/utility/MyUtility.h"
#include <format>

using namespace LWP::Base;
using namespace LWP::Utility;

PSO& PSO::Init(ID3D12RootSignature* root, DXC* dxc) {
	// ルートシグネチャをセットする
	desc_.pRootSignature = root;
	// DXCもセットしておく
	dxc_ = dxc;

	// -- デフォルトのパラメータセットをしておく -- //

	desc_.pRootSignature = root;	// RootSignature
	desc_.InputLayout = CreateInputLayout();		// InputLayout
	desc_.BlendState = CreateBlendState();			// BlendState
	desc_.RasterizerState = CreateRasterizerState();	// RasterizerState
	// VertexShaderとPixelShaderはデフォルトで空にしておく
	desc_.DepthStencilState = CreateDepthStencilState();
	desc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	desc_.NumRenderTargets = 1;
	desc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	desc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
	desc_.SampleDesc.Count = 1;
	desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	return *this;
}
PSO& PSO::SetInputLayout() {
	/* 頂点はバッファーで送信するので、InputLayoutは不要
	// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	*/

	// 虚無をセットしておく
	desc_.InputLayout = D3D12_INPUT_LAYOUT_DESC();
	return *this;
}
PSO& PSO::SetBlendState() {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 透明度のブレンドを設定
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// セット
	desc_.BlendState = blendDesc;
	return *this;
}
PSO& PSO::SetRasterizerState(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = cullMode;
	// 埋め立てモード
	rasterizerDesc.FillMode = fillMode;

	// セット
	desc_.RasterizerState = rasterizerDesc;
	return *this;
}
PSO& PSO::SetVertexShader(std::string filePath) {
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }
	
	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = CompileShader(Utility::ConvertString("resources/system/shaders/" + filePath), L"vs_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(blob != nullptr);

	// セット
	desc_.VS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetPixelShader(std::string filePath) {
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }

	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = CompileShader(Utility::ConvertString("resources/system/shaders/" + filePath), L"ps_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(blob != nullptr);

	// セット
	desc_.PS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetDepthStencilState() {
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true; // Depthの機能を有効化する
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // 書き込みします
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数はLessEqual（近ければ描画される）
	
	// セット
	desc_.DepthStencilState = depthStencilDesc;
	return *this;
}
PSO& PSO::SetDSVFormat(DXGI_FORMAT format) {
	// セット
	desc_.DSVFormat = format;
	return *this;
}
void PSO::Build(ID3D12Device* device) {
	// 実際に生成
	HRESULT hr = device->CreateGraphicsPipelineState(&desc_, IID_PPV_ARGS(&state_));
	assert(SUCCEEDED(hr));
}

/*
PSO& PSO::Initialize(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc, RasterizerState rState, UINT vs, UINT ps, DepthFormat df) {
	HRESULT hr = S_FALSE;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader = CreateVertexShader(dxc, vs);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader = CreatePixelShader(dxc, ps);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = root;	// RootSignature
	graphicsPipelineStateDesc.InputLayout = CreateInputLayout();		// InputLayout
	graphicsPipelineStateDesc.BlendState = CreateBlendState();			// BlendState
	graphicsPipelineStateDesc.RasterizerState = CreateRasterizerState(rState);	// RasterizerState
	if (vs != 0) {
		graphicsPipelineStateDesc.VS = { vertexShader->GetBufferPointer(),vertexShader->GetBufferSize() };	// VertexShader
	}
	if (ps != 0) {
		graphicsPipelineStateDesc.PS = { pixelShader->GetBufferPointer(),pixelShader->GetBufferSize() };	// PixelShader
	}
	graphicsPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	switch (df)
	{
		default:
		case LWP::Base::DepthFormat::D24_UNORM_S8_UINT:
			graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case LWP::Base::DepthFormat::D32_FLOAT:
			graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			break;
	}
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&state_));
	assert(SUCCEEDED(hr));
}

void PSO::InitializeForShadow(ID3D12Device* device, ID3D12RootSignature* root, DXC* dxc) {
	Initialize(device, root, dxc, 
		{RasterizerState::CullMode::Front, RasterizerState::FillMode::Solid },
		2, 0, DepthFormat::D32_FLOAT);
}
*/

D3D12_INPUT_LAYOUT_DESC PSO::CreateInputLayout() {
	/* 頂点はバッファーで送信するので、InputLayoutは不要
	// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	*/

	return D3D12_INPUT_LAYOUT_DESC();
}
D3D12_BLEND_DESC PSO::CreateBlendState() {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 透明度のブレンドを設定
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}
D3D12_RASTERIZER_DESC PSO::CreateRasterizerState() {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 埋め立てモード
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc;
}
D3D12_DEPTH_STENCIL_DESC PSO::CreateDepthStencilState() {
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true; // Depthの機能を有効化する
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // 書き込みします
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数はLessEqual（近ければ描画される）
	return depthStencilDesc;
}

IDxcBlob* PSO::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

	/*-- 1.hlslファイルを読む --*/

	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
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
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}


	/*-- 4.Compile結果を受け取って返す --*/

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"CompileSucceeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}