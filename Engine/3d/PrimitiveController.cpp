#include "3d.h"
#include <cassert>
#include <format>
#include "../utility/MyUtility.h"

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Math;

void Controller::Initialize(DirectXCommon* DirectXCommon) {
	directXCommon_ = DirectXCommon;

	InitialzieDXC();
	CreateGraphicsPipeLineState();
	CreateConstantBuffer();
	CreatePrimitiveVertex();
}

void Controller::Reset() {
	vertexIndex = 0;
}

void Controller::Draw(Vertex* vertex, int vertexCount, FillMode fillMode) {
	// 最大数を超えていないかチェック
	assert(vertexIndex < kMaxVertexCount);
	vertex;
	// 1ループで三角形を１つ描画
	for (int i = 0; i < vertexCount - 2; i++) {
		// primitiveVertexに座標を代入
		//primitiveVertex_->vertexData_[vertexIndex].position = { vertex[0].position.x,vertex[0].position.y,vertex[0].position.z,1.0f };
		//primitiveVertex_->vertexData_[vertexIndex].color = vertex[0].color.GetVector4();
		//primitiveVertex_->vertexData_[vertexIndex + 1].position = { vertex[i + 1].position.x,vertex[i + 1].position.y,vertex[i + 1].position.z,1.0f };
		//primitiveVertex_->vertexData_[vertexIndex + 1].color = vertex[i + 1].color.GetVector4();
		//primitiveVertex_->vertexData_[vertexIndex + 2].position = { vertex[i + 2].position.x,vertex[i + 2].position.y,vertex[i + 2].position.z,1.0f };
		//primitiveVertex_->vertexData_[vertexIndex + 2].color = vertex[i + 2].color.GetVector4();

		primitiveVertex_->vertexData_[vertexIndex].position = { -0.5f,-0.5f,0.0f,1.0f };
		primitiveVertex_->vertexData_[vertexIndex].color = { 255.0f,0.0f,0.0f,255.0f };
		primitiveVertex_->vertexData_[vertexIndex + 1].position = { -0.5f,0.5f,0.0f,1.0f };
		primitiveVertex_->vertexData_[vertexIndex + 1].color = { 0.0f,0.0f,0.0f,255.0f };
		primitiveVertex_->vertexData_[vertexIndex + 2].position = { -0.2f,0.0f,0.0f,1.0f };
		primitiveVertex_->vertexData_[vertexIndex + 2].color = { 0.0f,0.0f,0.0f,255.0f };

		// コマンドを積む
		ID3D12GraphicsCommandList* commandList = directXCommon_->GetCommandList();
		// RootSignatureを設定。PSOに設定してるけど別途設定が必要
		commandList->SetGraphicsRootSignature(pipelineSet_->rootSignature_.Get());
		// PSOを設定
		switch (fillMode)
		{
			case FillMode::WireFrame:
				commandList->SetPipelineState(pipelineSet_->graphicsPipelineStateWireFrame_.Get());
				break;
			case FillMode::Fill:
			default:
				commandList->SetPipelineState(pipelineSet_->graphicsPipelineStateFill_.Get());
				break;
		}
		commandList->IASetVertexBuffers(0, 1, &primitiveVertex_->vertexBufferView_);	// VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, cBuffer_->wvpResource_->GetGPUVirtualAddress());
		// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス、インスタンスについては今後
		commandList->DrawInstanced(3, 1, vertexIndex, 0);

		vertexIndex += 3;
	}
}


void Controller::InitialzieDXC() {
	HRESULT hr = S_FALSE;

	dxc_ = std::make_unique<DXC>();

	// DxcUtilsを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_->dxcUtils_));
	assert(SUCCEEDED(hr));
	// DxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_->dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxc_->dxcUtils_->CreateDefaultIncludeHandler(&dxc_->includeHandler_);
	assert(SUCCEEDED(hr));
}

#pragma region PipelineSet

#pragma region PSO生成関連

void Controller::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数設定できるように配列。今回は結果1つなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ
	
	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		MyUtility::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = directXCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet_->rootSignature_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}
D3D12_INPUT_LAYOUT_DESC Controller::CreateInputLayout() {
	// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	return inputLayoutDesc;
}
D3D12_BLEND_DESC Controller::CreateBlendState() {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 透明度のブレンドを設定（機能していないので一時的にコメントアウト）
	//blendDesc.RenderTarget[0].BlendEnable = true;
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	return blendDesc;
}
D3D12_RASTERIZER_DESC Controller::CreateRasterizerState() {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 埋め立てで設定
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	
	return rasterizerDesc;
}
IDxcBlob* Controller::CreateVertexShader() {
	// シェーダーをコンパイルする
	IDxcBlob* vertexShaderBlob{};
	vertexShaderBlob = CompileShader(L"./Engine/resources/Object3d.VS.hlsl", L"vs_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(vertexShaderBlob != nullptr);
	return vertexShaderBlob;
}
IDxcBlob* Controller::CreatePixelShader() {
	// シェーダーをコンパイルする
	IDxcBlob* pixelShaderBlob{};
	pixelShaderBlob = CompileShader(L"./Engine/resources/Object3d.PS.hlsl", L"ps_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(pixelShaderBlob != nullptr);
	return pixelShaderBlob;
}

#pragma endregion

void Controller::CreateGraphicsPipeLineState() {
	HRESULT hr = S_FALSE;

	pipelineSet_ = std::make_unique<PipelineSet>();

	CreateRootSignature();

	IDxcBlob* vertexShader = CreateVertexShader();
	IDxcBlob* pixelShader = CreatePixelShader();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipelineSet_->rootSignature_.Get();	// RootSignature
	graphicsPipelineStateDesc.InputLayout = CreateInputLayout();		// InputLayout
	graphicsPipelineStateDesc.BlendState = CreateBlendState();			// BlendState
	graphicsPipelineStateDesc.RasterizerState = CreateRasterizerState();	// RasterizerState
	graphicsPipelineStateDesc.VS = { vertexShader->GetBufferPointer(),vertexShader->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShader->GetBufferPointer(),pixelShader->GetBufferSize() };	// PixelShader
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineSet_->graphicsPipelineStateFill_));
	assert(SUCCEEDED(hr));
	// ワイヤーフレームモードも生成
	graphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineSet_->graphicsPipelineStateWireFrame_));
	assert(SUCCEEDED(hr));

	vertexShader->Release();
	pixelShader->Release();
}

#pragma endregion

void Controller::CreateConstantBuffer() {
	
	cBuffer_ = std::make_unique<CBuffer>();

	// wvpのリソースを作る。サイズはMatrix4x4 1つ分
	cBuffer_->wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));
	// データを書き込む
	cBuffer_->wvpData_ = nullptr;
	// 書き込むためのアドレスを取得
	cBuffer_->wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&cBuffer_->wvpData_));
	// 単位行列を書き込んでおく
	*cBuffer_->wvpData_ = Matrix4x4::CreateIdentity4x4();
}

void Controller::CreateVertexTriangleBufferView() {
	// 頂点バッファビューを作成する
	// リソースの先頭アドレスから使う
	primitiveVertex_->vertexBufferView_.BufferLocation = primitiveVertex_->vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	primitiveVertex_->vertexBufferView_.SizeInBytes = sizeof(VectorPosColor) * kMaxVertexCount;
	// 1頂点あたりのサイズ
	primitiveVertex_->vertexBufferView_.StrideInBytes = sizeof(VectorPosColor);
}

void Controller::CreatePrimitiveVertex() {

	primitiveVertex_ = std::make_unique<PrimitiveVertex>();

	primitiveVertex_->vertexResource_ = CreateBufferResource(sizeof(VectorPosColor) * kMaxVertexCount);
	CreateVertexTriangleBufferView();
	// 書き込むためのアドレスを取得
	primitiveVertex_->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&primitiveVertex_->vertexData_));
}


ID3D12Resource* Controller::CreateBufferResource(size_t sizeInBytes) {
	HRESULT hr = S_FALSE;

	ID3D12Resource* vertexResource;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	hr = directXCommon_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

IDxcBlob* Controller::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

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


