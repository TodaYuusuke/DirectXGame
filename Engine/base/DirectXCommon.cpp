#include "DirectXCommon.h"
#include "../utility/MyUtility.h"

#include <cassert>
#include <format>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Utility;

void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
	textureIndex_ = 0;

	// DXGIデバイス初期化
	InitializeDXGIDevice();

	kDescriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	kDescriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	kDescriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	// フェンス生成
	CreateFence();

	// 深度バッファ生成
	CreateDepthStencilTextureResource(backBufferWidth, backBufferHeight);
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList_->ResourceBarrier(1, &barrier);

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle = GetDescriptorHandleIncrementSize(
		rtvHeap_->GetCPUDescriptorHandleForHeapStart(), backBufferIndex,
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
	// 描画先のRTVとDSVを設定する
	commandList_->OMSetRenderTargets(1, &handle, false, &dsvHandle);
	
	// 全画面クリア
	ClearRenderTarget();
	// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(winApp_->GetClientWidth());
	viewport.Height = static_cast<float>(winApp_->GetClientHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// viewportを設定
	commandList_->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = winApp_->GetClientWidth();
	scissorRect.top = 0;
	scissorRect.bottom = winApp_->GetClientHeight();
	// Scirssorを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw() {
	HRESULT hr = S_FALSE;

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList_->ResourceBarrier(1, &barrier);


	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));


	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);


	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(event != nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocateor_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocateor_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::ClearRenderTarget() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle = GetDescriptorHandleIncrementSize(
		rtvHeap_->GetCPUDescriptorHandleForHeapStart(), backBufferIndex,
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(handle, clearColor, 0, nullptr);
}

ID3D12Resource* DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metadata) {
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
	hr = device_.Get()->CreateCommittedResource(
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

int DirectXCommon::UploadTextureData(ID3D12Resource* texture, D3D12_GPU_DESCRIPTOR_HANDLE* textureSRVHandleGPU, const DirectX::ScratchImage& mipImages) {
	HRESULT hr = S_FALSE;

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		hr = texture->WriteToSubresource(
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

	// indexを+1
	textureIndex_++;

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = GetCPUDescriptorHandle(srvHeap_.Get(), kDescriptorSizeSRV, textureIndex_);
	*textureSRVHandleGPU = GetGPUDescriptorHandle(srvHeap_.Get(), kDescriptorSizeSRV, textureIndex_);
	// SRVの生成
	device_.Get()->CreateShaderResourceView(texture, &srvDesc, textureSRVHandleCPU);
	return textureIndex_;
}



void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr = S_FALSE;

#ifdef _DEBUG

	// デバッグレイヤーをオンに
	ComPtr<ID3D12Debug1> debugController_;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		// デバッグレイヤーを有効化する
		debugController_->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController_->SetEnableGPUBasedValidation(true);
	}

#endif // _DEBUG

	// DXGIファクトリーの生成
	// HRESULTはWindow系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));


	// 使用するアダプタ用の変数
	ComPtr<IDXGIAdapter4> useAdapter;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));	// 取得できないのは一大事
		// ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力、wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;	// ソフトウェアアダプタの場合は見なかったことにする
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);


	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	const char* featureLevelStrings[] = { "12.2","12,1","12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");	// 初期化完了のログをだす

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Window11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverfloaw.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}

#endif // _DEBUG
}

void DirectXCommon::InitializeCommand() {
	HRESULT hr = S_FALSE;

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドアロケーターを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocateor_));
	// コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocateor_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	// スワップチェーンを生成する
	swapChainDesc_.Width = winApp_->GetClientWidth();				// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = winApp_->GetClientHeight();				// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;								// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr));

	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
}

void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr = S_FALSE;

	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	// SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	srvHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);


	// SwapChainからResourceを引っ張ってくる
	backBuffers_.resize(2);
	for (int i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));

		// ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetDescriptorHandleIncrementSize(
			rtvHeap_->GetCPUDescriptorHandleForHeapStart(), i,
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);

		// レンダーターゲットビューの設定
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc_, handle);
	}
}

void DirectXCommon::CreateFence() {
	HRESULT hr = S_FALSE;

	// 初期値0でFenceを作る
	fenceVal_ = 0;
	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	HRESULT hr = S_FALSE;

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width; // Textureの幅
	resourceDesc.Height = height; // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度地のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Resourceの生成
	hr = device_.Get()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&depthStencilResource_) // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	
	// DescriptorHeapを作成
	dsvHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本敵にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	//DSVHeapの先頭にDSVを作る
	device_.Get()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvHeap_.Get()->GetCPUDescriptorHandleForHeapStart());
}



ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	HRESULT hr = S_FALSE;

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetDescriptorHandleIncrementSize(const D3D12_CPU_DESCRIPTOR_HANDLE &other, int offsetInDescriptors, UINT descriptorIncrementSize) {
	// ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	if (offsetInDescriptors <= 0) {
		handle = other;
	}
	else {
		handle.ptr = other.ptr + (descriptorIncrementSize * offsetInDescriptors);
	}

	return handle;
}

D3D12_RESOURCE_BARRIER DirectXCommon::MakeResourceBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = stateBefore;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateAfter = stateAfter;

	return barrier;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

