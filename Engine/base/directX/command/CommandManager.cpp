#include "CommandManager.h"
#include <cassert>
#include <format>
#include "../Engine/utility/MyUtility.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/object/core/Camera.h"
#include "../Engine/object/core/light/DirectionLight.h"
#include "../Engine/object/core/light/PointLight.h"

#include <Config.h>
namespace RenderingPara = LWP::Config::Rendering;
namespace FPSPara = LWP::Config::FrameRate;

#include <Adapter.h>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void CommandManager::Initialize(ID3D12Device* device) {
	HRESULT hr = S_FALSE;
	device_ = device;

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));	// コマンドキューの生成がうまくいかなかったので起動できない
	// コマンドアロケーターを生成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));	// コマンドアロケーターの生成がうまくいかなかったので起動できない
	// コマンドリストを生成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));	// コマンドリストの生成がうまくいかなかったので起動できない

	// 初期値0でFenceを作る
	fenceVal_ = 0;
	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// シェーダーコンパイラ生成
	InitializeDXC();
	// RootSignature生成
	CreateRootSignature();
}

void CommandManager::SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
	rtv_ = rtv;
	dsv_ = dsv;
	srv_ = srv;

	// コマンド用クラス実態宣言
	
	// 本描画
	for (int i = 0; i < lwpC::Rendering::kMaxRenderingCount; i++) {
		mainCommands_.push_back(std::make_unique<MainCommand>());
		mainCommands_.back()->SetDescriptorHeap(rtv_, dsv_, srv_);
		mainCommands_.back()->Initialize(device_, dxc_.get(), rootSignature_.Get());
	}
	// シャドウマップコマンド用の実体
	for (int i = 0; i < lwpC::Shadow::kMaxShadowMap; i++) {
		shadowCommands_.push_back(std::make_unique<ShadowMapCommand>());
		shadowCommands_.back()->SetDescriptorHeap(rtv_, dsv_, srv_);
		shadowCommands_.back()->Initialize(device_, dxc_.get(), rootSignature_.Get());
	}
	
	// グラフィックリソースを作成
	CreateStructuredBufferResources();

	// SRVを登録してからでないとテクスチャが読み込めないので、
	// ここでデフォルトテクスチャを読み込む
	defaultTexture_ = LWP::Resource::LoadTextureLongPath("resources/system/texture/white.png");
}

void CommandManager::PreDraw() {/* -- DrawCallを圧縮したのでそのうち削除 -- */}

void CommandManager::DrawCall() {
	HRESULT hr;

	// コマンドリストの実行とリセットのラムダ式（渡す引数が多いのでこちらで定義し、ラムダを渡す）
	std::function<void()> ExecuteLambda = [&]() {
		// GPUにコマンドリストの実行を行わせる
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(1, commandLists);

		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue_->Signal(fence_.Get(), ++fenceVal_);
		if (fence_->GetCompletedValue() != fenceVal_) {
			HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(event != nullptr);
			fence_->SetEventOnCompletion(fenceVal_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// 次のコマンドリストを準備
		hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
	};
	std::function<void()> dummy = [&]() {};

	// コマンドのDrawを呼び出すラムダ式（引数で渡すのは面倒なのでラムダで指定）
	std::function<void(ICommand*)> DrawLambda = [&](ICommand* cmd) {
		cmd->Draw(rootSignature_.Get(), commandList_.Get(), {
			commonDataResourceBuffer_->view_,
			vertexData_->GetView(),
			transformData_->GetView(),
			materialData_->GetView(),
			directionLightResourceBuffer_->view_,
			pointLightResourceBuffer_->view_,
			textureResourceBuffer_->view_,
			directionLightResourceBuffer_->shadowMap_[0].view_,
			pointLightResourceBuffer_->shadowMap_[0].view_
		});
	};

	// シャドウマップのDrawCallを積む
	for (int i = 0; i < shadowCount_; i++) {
		DrawLambda(shadowCommands_[i].get());
	}
	// 本描画
	for (int i = 0; i < mainCount_; i++) {
		DrawLambda(mainCommands_[i].get());
	}

	// 実行
	ExecuteLambda();

	// GPUとOSに画面の交換を行うよう通知する
	rtv_->GetSwapChain()->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
}

void CommandManager::PostDraw() {/* -- DrawCallを圧縮したのでそのうち削除 -- */}

void CommandManager::Reset() {
	// コマンドをリセット
	for (int i = 0; i < shadowCount_; i++) {
		shadowCommands_[i]->Reset();
	}
	shadowCount_ = 0;
	for (int i = 0; i < mainCount_; i++) {
		mainCommands_[i]->Reset();
	}
	mainCount_ = 0;

	// 使用量をリセット
	vertexData_->Reset();
	transformData_->Reset();
	commonDataResourceBuffer_->data_->directionLight = 0;
	commonDataResourceBuffer_->data_->pointLight = 0;
	materialData_->Reset();
	directionLightResourceBuffer_->usedCount_ = 0;
	pointLightResourceBuffer_->usedCount_ = 0;
}

void CommandManager::SetCameraViewProjection(const Object::Camera* camera) {
	// カメラ視点の描画を予約
	mainCommands_[mainCount_++]->SetDrawTarget(camera->GetViewProjection());
}

void CommandManager::SetDirectionLightData(const Object::DirectionLight* light, const Math::Matrix4x4& viewProjection) {
	DirectionalLightStruct newData{};
	newData.vp = viewProjection;	// viewProjection
	newData.color = light->color.GetVector4();		// ライトの色
	newData.direction = (Vector3{ 0.0f,0.0f,1.0f } * Matrix4x4::CreateRotateXYZMatrix(light->rotation)).Normalize();	// ライトの向き
	newData.intensity = light->intensity;	// 輝度
	// データを登録
	directionLightResourceBuffer_->data_[directionLightResourceBuffer_->usedCount_++] = newData;

	shadowCommands_[shadowCount_++]->SetDrawTarget(
		viewProjection,
		directionLightResourceBuffer_->shadowMap_->resource_.Get(),
		directionLightResourceBuffer_->shadowMap_->dsvIndex_
	);
	// 使用カウント+1
	commonDataResourceBuffer_->data_->directionLight++;
}
void CommandManager::SetPointLightData(const Object::PointLight* light, const Math::Matrix4x4* viewProjections) {
	// 点光源のデータをリソースにコピー
	PointLightStruct newData{};

	newData.color = light->color.GetVector4();	// ライトの色
	newData.position = light->transform.GetWorldPosition();	// ライトのワールド座標
	newData.intensity = light->intensity;		// 輝度
	newData.radius = light->radius;			// ライトの届く最大距離
	newData.decay = light->decay;			// 減衰率
	// データを登録
	pointLightResourceBuffer_->data_[pointLightResourceBuffer_->usedCount_++] = newData;

	// シャドウマップのレンダリングを予約
	for (int i = 0; i < 6; i++) {
		shadowCommands_[shadowCount_++]->SetDrawTarget(
			viewProjections[i],
			pointLightResourceBuffer_->shadowMap_->resource_.Get(),
			pointLightResourceBuffer_->shadowMap_->dsvIndex_[i]
		);
	}
	// 使用カウント+1
	commonDataResourceBuffer_->data_->pointLight++;
}
int CommandManager::CreateTextureResource(const DirectX::ScratchImage& image) {
	textureResourceBuffer_->resource_.push_back(CreateBufferResource(image.GetMetadata()));
	UploadTextureData(image);
	return textureResourceBuffer_->usedCount_++;
}

void CommandManager::SetDrawData(Primitive::IPrimitive* primitive) {

	uint32_t startIndexNum = vertexData_->GetCount();
	
	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		VertexStruct ver;
		ver = primitive->vertices[i];
		if(primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			ver.color_ = primitive->commonColor->GetVector4();
		vertexData_->AddData(ver);	// データを追加
	}
	// ワールドトランスフォームをデータに登録
	uint32_t worldMatrix = transformData_->GetCount();
	transformData_->AddData(primitive->transform.GetWorldMatrix());
	// マテリアルをデータに登録
	uint32_t material = materialData_->GetCount();
	MaterialStruct m;
	m = primitive->material;
	materialData_->AddData(m);
	// テクスチャのインデックスを貰う
	uint32_t tex2d = defaultTexture_->GetIndex();
	if (primitive->texture.t != nullptr) {
		tex2d = primitive->texture.t->GetIndex();
	}

	// Indexの分だけIndexInfoを求める
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		IndexInfoStruct indexInfo = {
			startIndexNum + primitive->indexes[i],
			worldMatrix,
			material,
			tex2d,
			primitive->isUI
		};

		// メインコマンドにデータをセット
		for (int s = 0; s < mainCount_; s++) {
			mainCommands_[s]->SetDrawData(indexInfo);
		}
		// シャドウマップにも描画！
		if (primitive->material.enableLighting) {
			for (int s = 0; s < shadowCount_; s++) {
				shadowCommands_[s]->SetDrawData(indexInfo);
			}
		}
	}
}


void CommandManager::InitializeDXC() {
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

void CommandManager::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	// RootParameter作成。複数設定できるように配列。今回は結果12つ
	D3D12_ROOT_PARAMETER rootParameters[11] = {};
	// テクスチャ用サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[3] = {};
	// 配列用のRangeDesc
	D3D12_DESCRIPTOR_RANGE descRange[1] = {};	// DescriptorRangeを作成
	descRange[0].BaseShaderRegister = 0; // 0から始まる
	descRange[0].NumDescriptors = 1; // 数は1つ
	descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootSignatureにrootParametersを登録
	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

	// RootSignatureにサンプラーを登録
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	// ** 両方で使うデータ ** //

	// ストラクチャーバッファーのインデックス
	D3D12_DESCRIPTOR_RANGE indexDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	indexDesc[0].BaseShaderRegister = 0; // レジスタ番号は0
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = indexDesc; // Tabelの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(indexDesc); // Tableで利用する数
	// 描画するViewProjection
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	// 全描画で共通のデータ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド

	// ** VertexShaderで使うデータ ** //

	// 頂点データ
	D3D12_DESCRIPTOR_RANGE vertexDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	vertexDesc[0].BaseShaderRegister = 1; // レジスタ番号は1
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[3].DescriptorTable.pDescriptorRanges = vertexDesc; // Tabelの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(vertexDesc); // Tableで利用する数

	// WorldTransform
	D3D12_DESCRIPTOR_RANGE wtfDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	wtfDesc[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[4].DescriptorTable.pDescriptorRanges = wtfDesc; // Tabelの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(wtfDesc); // Tableで利用する数


	// ** PixelShaderで使うデータ ** //

	// マテリアル
	D3D12_DESCRIPTOR_RANGE materialDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	materialDesc[0].BaseShaderRegister = 1; // レジスタ番号は1
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = materialDesc; // Tabelの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(materialDesc); // Tableで利用する数

	// 平行光源
	D3D12_DESCRIPTOR_RANGE dirLightDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	dirLightDesc[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].DescriptorTable.pDescriptorRanges = dirLightDesc; // Tabelの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(dirLightDesc); // Tableで利用する数
	// 点光源
	D3D12_DESCRIPTOR_RANGE pointLightDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	pointLightDesc[0].BaseShaderRegister = 3; // レジスタ番号は3
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = pointLightDesc; // Tabelの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(pointLightDesc); // Tableで利用する数


#pragma region テクスチャ実装
	// Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号は0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// テクスチャ
	D3D12_DESCRIPTOR_RANGE textureDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	textureDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	textureDesc[0].RegisterSpace = 1; // スペースは1
	textureDesc[0].NumDescriptors = kMaxTexture;	// 最大数を定義
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[8].DescriptorTable.pDescriptorRanges = textureDesc; // Tabelの中身の配列を指定
	rootParameters[8].DescriptorTable.NumDescriptorRanges = _countof(textureDesc); // Tableで利用する数
#pragma endregion

#pragma region 平行光源のシャドウマップ
	// Samplerの設定
	staticSamplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // バイアスをかけて線形補間
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数を設定
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1; // レジスタ番号は1
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	D3D12_DESCRIPTOR_RANGE dirShadowDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	dirShadowDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	dirShadowDesc[0].RegisterSpace = 2; // スペースは2
	dirShadowDesc[0].NumDescriptors = lwpC::Shadow::Direction::kMaxCount;	// 最大数を定義
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[9].DescriptorTable.pDescriptorRanges = dirShadowDesc; // Tabelの中身の配列を指定
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(dirShadowDesc); // Tableで利用する数
#pragma endregion

#pragma region 点光源のシャドウマップ
	// Samplerの設定
	staticSamplers[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // バイアスをかけて線形補間
	staticSamplers[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数を設定
	staticSamplers[2].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[2].ShaderRegister = 2; // レジスタ番号は2
	staticSamplers[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	D3D12_DESCRIPTOR_RANGE pointShadowDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	pointShadowDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	pointShadowDesc[0].RegisterSpace = 3; // スペースは2
	pointShadowDesc[0].NumDescriptors = lwpC::Shadow::Point::kMaxCount * 6;	// 最大数を定義
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[10].DescriptorTable.pDescriptorRanges = pointShadowDesc; // Tabelの中身の配列を指定
	rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(pointShadowDesc); // Tableで利用する数
#pragma endregion


	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}

void CommandManager::CreateStructuredBufferResources() {
	// 共通のSRV用Desc
	D3D12_SHADER_RESOURCE_VIEW_DESC commonDesc{};
	commonDesc.Format = DXGI_FORMAT_UNKNOWN;
	commonDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	commonDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	commonDesc.Buffer.FirstElement = 0;
	commonDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


	// 頂点データ
	vertexData_ = std::make_unique<IStructured<VertexStruct>>(device_, srv_, RenderingPara::kMaxVertex);
	// WorldTransformデータ
	transformData_ = std::make_unique<IStructured<Math::Matrix4x4>>(device_, srv_, RenderingPara::kMaxMatrix);

	// 構造体のカウント
	commonDataResourceBuffer_ = std::make_unique<CommonDataResourceBuffer>();
	commonDataResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(CommonData));
	commonDataResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&commonDataResourceBuffer_->data_));
	commonDataResourceBuffer_->view_ = commonDataResourceBuffer_->resource_->GetGPUVirtualAddress();
	// 2D用のViewProjectionを作成しておく
	commonDataResourceBuffer_->data_->vp2D = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, LWP::Info::GetWindowWidthF(), LWP::Info::GetWindowHeightF(), 0.0f, 100.0f);
	// マテリアルデータ
	materialData_ = std::make_unique<IStructured<MaterialStruct>>(device_, srv_, RenderingPara::kMaxMaterial);
	
	// 平行光源
	directionLightResourceBuffer_ = std::make_unique<DirectionLightResourceBuffer>();
	directionLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(DirectionalLightStruct) * lwpC::Shadow::Point::kMaxCount);
	directionLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC dirLightDesc = { commonDesc };
	dirLightDesc.Buffer.NumElements = lwpC::Shadow::Direction::kMaxCount;
	dirLightDesc.Buffer.StructureByteStride = sizeof(DirectionalLightStruct);
	directionLightResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(directionLightResourceBuffer_->resource_.Get(), &dirLightDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// シャドウマップも作る
	directionLightResourceBuffer_->shadowMap_ = new DirectionShadowMapStruct[lwpC::Shadow::Direction::kMaxCount];
	for (int i = 0; i < lwpC::Shadow::Direction::kMaxCount; i++) {
		directionLightResourceBuffer_->shadowMap_[i].resource_ = dsv_->CreateDirectionShadowMap(&directionLightResourceBuffer_->shadowMap_[i].dsvIndex_, &directionLightResourceBuffer_->shadowMap_[i].view_);
	}
	/*directionLightResourceBuffer_ = std::make_unique<DirectionLightResourceBuffer>();
	directionLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(DirectionalLightStruct) * lwpC::Shadow::Direction::kMaxCount);
	directionLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightResourceBuffer_->data_));
	directionLightResourceBuffer_->view_ = directionLightResourceBuffer_->resource_->GetGPUVirtualAddress();
	// シャドウマップも作る
	directionLightResourceBuffer_->shadowMap_ = new DirectionShadowMapStruct[lwpC::Shadow::Direction::kMaxCount];
	directionLightResourceBuffer_->shadowMap_[0].resource_ = dsv_->CreateDirectionShadowMap(&directionLightResourceBuffer_->shadowMap_[0].dsvIndex_, &directionLightResourceBuffer_->shadowMap_[0].view_);*/

	// 点光源
	pointLightResourceBuffer_ = std::make_unique<PointLightResourceBuffer>();
	pointLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(PointLightStruct) * lwpC::Shadow::Point::kMaxCount);
	pointLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC pointLightDesc = { commonDesc };
	pointLightDesc.Buffer.NumElements = lwpC::Shadow::Point::kMaxCount;
	pointLightDesc.Buffer.StructureByteStride = sizeof(PointLightStruct);
	pointLightResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(pointLightResourceBuffer_->resource_.Get(), &pointLightDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// シャドウマップも作る
	pointLightResourceBuffer_->shadowMap_ = new PointShadowMapStruct[lwpC::Shadow::Point::kMaxCount];
	for (int i = 0; i < lwpC::Shadow::Point::kMaxCount; i++) {
		pointLightResourceBuffer_->shadowMap_[i].resource_ = dsv_->CreatePointShadowMap(pointLightResourceBuffer_->shadowMap_[i].dsvIndex_, &pointLightResourceBuffer_->shadowMap_[i].view_);
	}

	// テクスチャデータ
	textureResourceBuffer_ = std::make_unique<TextureResourceBuffer>();
	//textureResource_->resource_ = CreateBufferResource(sizeof(IndexInfoStrict) * kMaxTexture);
}

ID3D12Resource* CommandManager::CreateBufferResource(const DirectX::TexMetadata& metadata) {
	HRESULT hr = S_FALSE;

	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 2. 利用するHeapの設定。非常に特殊な運用。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 3. Resourceを生成する
	ID3D12Resource* resource = nullptr;
	hr = device_->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初回のResourceState。Textureは基本読むだけ
		nullptr,							// Clear最適地。使わないでnullptr
		IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}
void CommandManager::UploadTextureData(const DirectX::ScratchImage& mipImages) {
	HRESULT hr = S_FALSE;

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		hr = textureResourceBuffer_->resource_[textureResourceBuffer_->usedCount_]->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める（ImGuiとStructuredBufferたちが先頭を使っているので + ）
	UINT srvIndex = srv_->GetAndIncrement();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(srvIndex);
	// 初めてのテクスチャ生成ならviewを保存
	if (textureResourceBuffer_->usedCount_ == 0) {
		textureResourceBuffer_->view_ = srv_->GetGPUHandle(srvIndex);
	}
	// SRVの生成
	device_->CreateShaderResourceView(textureResourceBuffer_->resource_[textureResourceBuffer_->usedCount_].Get(), &srvDesc, textureSRVHandleCPU);
}