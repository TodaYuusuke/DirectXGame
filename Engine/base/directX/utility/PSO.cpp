#include "PSO.h"
#include "utility/MyUtility.h"
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
	blob = dxc_->CompileShader(Utility::ConvertString("resources/system/shaders/" + filePath), L"vs_6_0");
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
	blob = dxc_->CompileShader(Utility::ConvertString("resources/system/shaders/" + filePath), L"ps_6_0");
	assert(blob != nullptr);

	// セット
	desc_.PS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetDepthStencilState(bool enable) {
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = enable; // Depthの機能を有効化する
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
PSO& PSO::SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
	// セット
	desc_.PrimitiveTopologyType = type;
	return *this;
}
void PSO::Build(ID3D12Device* device) {
	HRESULT hr = S_FALSE;
	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&desc_, IID_PPV_ARGS(&state_));
	assert(SUCCEEDED(hr));
}

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
