#include "DrawingProcessor.h"
#include <cassert>
#include <format>
#include "MyUtility.h"


using namespace Microsoft::WRL;

DrawingProcessor* DrawingProcessor::GetInstance() {
	static DrawingProcessor instance;
	return &instance;
}

void DrawingProcessor::Initialize(DirectXCommon* DirectXCommon) {
	directXCommon_ = DirectXCommon;

	InitialzieDXC();
	CreateGraphicsPipeLineState();
	CreateVerTexTriangle();
}

void DrawingProcessor::DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color) {

	// 最大数を超えていないかチェック
	assert(vertexTriangle_->triangleCount_ < kMaxTriangleCount_);

	// Resourceにデータを書き込む 
	int index = vertexTriangle_->triangleCount_ * kVertexCountTriangle_;

	// vertexDataに座標を代入
	vertexTriangle_->vertexData_[index].position = { pos1.x,pos1.y,pos1.z,1.0f };
	vertexTriangle_->vertexData_[index].color = HexColorToVector4(color);
	vertexTriangle_->vertexData_[index + 1].position = { pos2.x,pos2.y,pos2.z,1.0f };
	vertexTriangle_->vertexData_[index + 1].color = HexColorToVector4(color);
	vertexTriangle_->vertexData_[index + 2].position = { pos3.x,pos3.y,pos3.z,1.0f };
	vertexTriangle_->vertexData_[index + 2].color = HexColorToVector4(color);
		
	// コマンドを積む
	ID3D12GraphicsCommandList* commandList = directXCommon_->GetCommandList();
	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	commandList->SetGraphicsRootSignature(pipelineSet_->rootSignature_.Get());
	commandList->SetPipelineState(pipelineSet_->graphicsPipelineState_.Get());	// PSOを設定
	commandList->IASetVertexBuffers(0, 1, &vertexTriangle_->vertexBufferView_);	// VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, vertexTriangle_->materialResource_->GetGPUVirtualAddress());
	// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス、インスタンスについては今後
	commandList->DrawInstanced(3, 1, vertexTriangle_->triangleCount_ * 3, 0);

	// 描画数+1
	vertexTriangle_->triangleCount_++;
}



void DrawingProcessor::InitialzieDXC() {
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

void DrawingProcessor::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数設定できるのに配列。今回は結果2つなので長さ2の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;		// SRVを使う（色データ用）
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド
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
D3D12_INPUT_LAYOUT_DESC DrawingProcessor::CreateInputLayout() {
	// 頂点レイアウト
	//static D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	//inputElementDescs[0].SemanticName = "POSITION";
	//inputElementDescs[0].SemanticIndex = 0;
	//inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[1].SemanticName = "COLOR";
	//inputElementDescs[1].SemanticIndex = 0;
	//inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	return inputLayoutDesc;
}
D3D12_BLEND_DESC DrawingProcessor::CreateBlendState() {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blendDesc;
}
D3D12_RASTERIZER_DESC DrawingProcessor::CreateRasterizerState() {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	
	return rasterizerDesc;
}
IDxcBlob* DrawingProcessor::CreateVertexShader() {
	// シェーダーをコンパイルする
	IDxcBlob* vertexShaderBlob{};
	vertexShaderBlob = CompileShader(L"./Resources/Object3d.VS.hlsl", L"vs_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(vertexShaderBlob != nullptr);
	return vertexShaderBlob;
}
IDxcBlob* DrawingProcessor::CreatePixelShader() {
	// シェーダーをコンパイルする
	IDxcBlob* pixelShaderBlob{};
	pixelShaderBlob = CompileShader(L"./Resources/Object3d.PS.hlsl", L"ps_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(pixelShaderBlob != nullptr);
	return pixelShaderBlob;
}

#pragma endregion

void DrawingProcessor::CreateGraphicsPipeLineState() {
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
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineSet_->graphicsPipelineState_));
	assert(SUCCEEDED(hr));

	vertexShader->Release();
	pixelShader->Release();
}

#pragma endregion

void DrawingProcessor::CreateVertexTriangleBufferView() {
	// 頂点バッファビューを作成する
	// リソースの先頭アドレスから使う
	vertexTriangle_->vertexBufferView_.BufferLocation = vertexTriangle_->vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexTriangle_->vertexBufferView_.SizeInBytes = sizeof(VectorPosColor) * kMaxTriangleCount_ * kVertexCountTriangle_;
	// 1頂点あたりのサイズ
	vertexTriangle_->vertexBufferView_.StrideInBytes = sizeof(VectorPosColor);
}

void DrawingProcessor::CreateVerTexTriangle() {

	vertexTriangle_ = std::make_unique<VertexTriangle>();

	vertexTriangle_->vertexResource_ = CreateBufferResource(sizeof(VectorPosColor) * kMaxTriangleCount_ * kVertexCountTriangle_);
	CreateVertexTriangleBufferView();
	// 書き込むためのアドレスを取得
	vertexTriangle_->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexTriangle_->vertexData_));

	// マテリアル用のリソースを作る。
	vertexTriangle_->materialResource_ = CreateBufferResource(sizeof(VectorPosColor) * kMaxTriangleCount_ * kVertexCountTriangle_);
	// 書き込むためのアドレスを取得
	vertexTriangle_->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexTriangle_->vertexData_->color));
}


ID3D12Resource* DrawingProcessor::CreateBufferResource(size_t sizeInBytes) {
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

IDxcBlob* DrawingProcessor::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

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

Vector4 DrawingProcessor::HexColorToVector4(unsigned int color) {
	Vector4 result;
	float* f[4] = { &result.w, &result.z, &result.y, &result.x };
	for (int i = 0; i < 4; i++) {
		*f[i] = (float)(color % 0x100 / 0xFF);
		color /= 0x100;
	}

	return result;
}
