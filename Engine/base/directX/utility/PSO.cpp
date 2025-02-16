#include "PSO.h"
#include "utility/MyUtility.h"
#include <format>

using namespace LWP::Base;
using namespace LWP::Utility;

PSO& PSO::Init(ID3D12RootSignature* root, Type type) {
	// タイプによって使用するdescを変更
	type_ = type;
	if (type_ == Type::Vertex) {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC& d = desc_.vertex;
		// ルートシグネチャをセットする
		d.pRootSignature = root;

		// -- デフォルトのパラメータセットをしておく -- //

		d.pRootSignature = root;	// RootSignature
		d.InputLayout = CreateInputLayout();		// InputLayout
		d.BlendState = CreateBlendState();			// BlendState
		d.RasterizerState = CreateRasterizerState();	// RasterizerState
		// VertexShaderとPixelShaderはデフォルトで空にしておく
		d.DepthStencilState = CreateDepthStencilState();
		d.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		// 書き込むRTVの情報
		d.NumRenderTargets = 1;
		d.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトポロジ（形状）のタイプ。三角形
		d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
		d.SampleDesc.Count = 1;
		d.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	}
	else if (type_ == Type::Compute) {
		D3D12_COMPUTE_PIPELINE_STATE_DESC& d = desc_.compute;
		d.pRootSignature = root;	// セットするのはRootSignatureのみ
	}
	else if(type_ == Type::Mesh){
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC& d = desc_.mesh;
		// ルートシグネチャをセットする
		d.pRootSignature = root;

		// -- デフォルトのパラメータセットをしておく -- //

		d.pRootSignature = root;	// RootSignature
		d.BlendState = CreateBlendState();			// BlendState
		d.RasterizerState = CreateRasterizerState();	// RasterizerState
		// VertexShaderとPixelShaderはデフォルトで空にしておく
		d.DepthStencilState = CreateDepthStencilState();
		d.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		// 書き込むRTVの情報
		d.NumRenderTargets = 1;
		d.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトポロジ（形状）のタイプ。三角形
		d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
		d.SampleDesc.Count = 1;
		d.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	}

	return *this;
}
PSO& PSO::SetInputLayout() {
	if (type_ == Type::Vertex) {
		/* 頂点はバッファーで送信するので、InputLayoutは不要
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
		desc_.vertex.InputLayout = D3D12_INPUT_LAYOUT_DESC();
	}
	else {
		// ComputeかMeshのときは使わないのでエラー
		assert(false);
	}

	return *this;
}
PSO& PSO::SetBlendState(BlendMode mode) {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 透明度のブレンドを設定
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	switch (mode) {
		default:
		case LWP::Base::PSO::BlendMode::Normal:
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case LWP::Base::PSO::BlendMode::Add:
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case LWP::Base::PSO::BlendMode::Subtract:
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case LWP::Base::PSO::BlendMode::Multiply:
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case LWP::Base::PSO::BlendMode::Screen:
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
	}

	// セット
	if (type_ == Type::Vertex) {
		desc_.vertex.BlendState = blendDesc;
	}
	else if (type_ == Type::Mesh) {
		desc_.mesh.BlendState = blendDesc;
	}

	return *this;
}
PSO& PSO::SetRasterizerState(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = cullMode;
	// 埋め立てモード
	rasterizerDesc.FillMode = fillMode;

	// セット
	if (type_ == Type::Vertex) {
		desc_.vertex.RasterizerState = rasterizerDesc;
	}
	else if (type_ == Type::Mesh) {
		desc_.mesh.RasterizerState = rasterizerDesc;
	}
	return *this;
}
PSO& PSO::SetAS(std::string filePath) {
	// Mesh以外ならセットしないのでエラー
	assert(type_ == Type::Mesh);
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }

	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = DXC::GetInstance()->CompileShader(Utility::ConvertString(filePath), L"as_6_5");
	assert(blob != nullptr);

	// セット
	desc_.mesh.AS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetSystemAS(std::string filePath) { return SetAS(kDirectoryPath + filePath); }
PSO& PSO::SetMS(std::string filePath) {
	// Mesh以外ならセットしないのでエラー
	assert(type_ == Type::Mesh);
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }

	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = DXC::GetInstance()->CompileShader(Utility::ConvertString(filePath), L"ms_6_5");
	assert(blob != nullptr);

	// セット
	desc_.mesh.MS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetSystemMS(std::string filePath) { return SetMS(kDirectoryPath + filePath); }
PSO& PSO::SetCS(std::string filePath) {
	// Compute以外ならセットしないのでエラー
	assert(type_ == Type::Compute);
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }

	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = DXC::GetInstance()->CompileShader(Utility::ConvertString(filePath), L"cs_6_6");
	assert(blob != nullptr);

	// セット
	desc_.compute.CS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetSystemCS(std::string filePath) { return SetCS(kDirectoryPath + filePath); }
PSO& PSO::SetVS(std::string filePath) {
	// Vertex以外ならセットしないのでエラー
	assert(type_ == Type::Vertex);
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }
	
	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = DXC::GetInstance()->CompileShader(Utility::ConvertString(filePath), L"vs_6_0");
	assert(blob != nullptr);

	// セット
	desc_.vertex.VS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	return *this;
}
PSO& PSO::SetSystemVS(std::string filePath) { return SetVS(kDirectoryPath + filePath); }
PSO& PSO::SetPS(std::string filePath) {
	// 空ならコンパイルしない
	if (filePath.empty()) { return *this; }

	// シェーダーをコンパイルする
	IDxcBlob* blob = nullptr;
	blob = DXC::GetInstance()->CompileShader(Utility::ConvertString(filePath), L"ps_6_5");
	assert(blob != nullptr);

	// セット
	if (type_ == Type::Vertex) {
		desc_.vertex.PS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	}
	else if (type_ == Type::Mesh) {
		desc_.mesh.PS = { blob->GetBufferPointer(),blob->GetBufferSize() };
	}

	return *this;
}
PSO& PSO::SetSystemPS(std::string filePath) { return SetPS(kDirectoryPath + filePath); }
PSO& PSO::SetDepthState(bool enable, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func) {
	D3D12_DEPTH_STENCIL_DESC depthDesc{};
	// 受け取る
	if (type_ == Type::Vertex) { depthDesc = desc_.vertex.DepthStencilState; }
	else if (type_ == Type::Mesh) { depthDesc = desc_.mesh.DepthStencilState; }

	depthDesc.DepthEnable = enable; // Depthの機能を有効化する
	depthDesc.DepthWriteMask = mask; // 書き込みします
	depthDesc.DepthFunc = func; // 比較関数
	
	// セット
	if (type_ == Type::Vertex) { desc_.vertex.DepthStencilState = depthDesc; }
	else if (type_ == Type::Mesh) { desc_.mesh.DepthStencilState = depthDesc; }
	return *this;
}
PSO& PSO::SetStencilState(bool enable,
	D3D12_DEPTH_STENCILOP_DESC front, D3D12_DEPTH_STENCILOP_DESC back) {
	D3D12_DEPTH_STENCIL_DESC stencilDesc{};
	// 受け取る
	if (type_ == Type::Vertex) { stencilDesc = desc_.vertex.DepthStencilState; }
	else if (type_ == Type::Mesh) { stencilDesc = desc_.mesh.DepthStencilState; }

	// StencilOP_Descの詳細
	// D3D12_STENCIL_OP StencilFailOp;		// ステンシルテストが失敗した時の動作
	// D3D12_STENCIL_OP StencilDepthFailOp; // 深度テストが失敗した時の動作
	// D3D12_STENCIL_OP StencilPassOp;		// ステンシル＆深度テストが成功した時の動作
	// D3D12_COMPARISON_FUNC StencilFunc;	// ステンシルテストの比較関数
	
	// ステンシルテスト
	stencilDesc.StencilEnable = enable;
	stencilDesc.StencilReadMask = 0xFF;
	stencilDesc.StencilWriteMask = 0xFF;
	// ステンシルテスト（前面ポリゴン）の設定
	stencilDesc.FrontFace = front;
	// ステンシルテスト（背面ポリゴン）の設定
	stencilDesc.BackFace = back;

	// セット
	if (type_ == Type::Vertex) { desc_.vertex.DepthStencilState = stencilDesc; }
	else if (type_ == Type::Mesh) { desc_.mesh.DepthStencilState = stencilDesc; }
	return *this;
}
PSO& PSO::SetDSVFormat(DXGI_FORMAT format) {
	// セット
	if (type_ == Type::Vertex) {
		desc_.vertex.DSVFormat = format;
	}
	else if (type_ == Type::Mesh) {
		desc_.mesh.DSVFormat = format;
	}

	return *this;
}
PSO& PSO::SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
	// セット
	if (type_ == Type::Vertex) {
		desc_.vertex.PrimitiveTopologyType = type;
	}
	else if (type_ == Type::Mesh) {
		desc_.mesh.PrimitiveTopologyType = type;
	}

	return *this;
}
void PSO::Build() {
	HRESULT hr = S_FALSE;
	ID3D12Device2* device = GPUDevice::GetInstance()->GetDevice();	// デバイスを取得

	// 実際に生成
	if (type_ == Type::Vertex) {
		hr = device->CreateGraphicsPipelineState(&desc_.vertex, IID_PPV_ARGS(&state_));
	}
	else if (type_ == Type::Compute) {
		hr = device->CreateComputePipelineState(&desc_.compute, IID_PPV_ARGS(&state_));
	}
	else if (type_ == Type::Mesh) {
		// PSOの型を指定形式に変換
		auto stream = CD3DX12_PIPELINE_MESH_STATE_STREAM(desc_.mesh);
		// PSOの設定を対応したものに変換する
		D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		streamDesc.pPipelineStateSubobjectStream = &stream;
		streamDesc.SizeInBytes = sizeof(stream);

		// 設定を元に実際に生成を行う
		hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&state_));
	}
	assert(SUCCEEDED(hr));
}

PSO& PSO::Copy(const PSO& source) {
	// タイプをコピー
	type_ = source.GetType();
	// 詳細をコピー
	desc_ = source.GetDesc();

	return *this;
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
