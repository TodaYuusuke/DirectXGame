#include "RootSignature.h"
#include "utility/MyUtility.h"

using namespace LWP::Base;

RootSignature& RootSignature::AddCBVParameter(int registerNum, ShaderVisibility visibility) {
	// 新しく登録するデータ
	D3D12_ROOT_PARAMETER newPara{};
	newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	newPara.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility);
	newPara.Descriptor.ShaderRegister = registerNum;
	// 登録
	parameters_.push_back(newPara);
	return *this;
}
RootSignature& RootSignature::AddTableParameter(int registerNum, ShaderVisibility visibility, int space, UINT maxSize) {
	// 新しく登録するTableの設定
	D3D12_DESCRIPTOR_RANGE desc = defaultTableDesc_;
	desc.BaseShaderRegister = registerNum;	// レジスタ番号を登録
	desc.RegisterSpace = space;	// スペースを設定（デフォは0）
	if (maxSize != 0) { desc.NumDescriptors = maxSize; }	// 0じゃないときのみ最大数を登録
	// 新しく登録するデータ
	D3D12_ROOT_PARAMETER newPara{};
	newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	newPara.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility);
	newPara.DescriptorTable.pDescriptorRanges = &desc; // Tabelの中身の配列を指定
	newPara.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数
	// 登録
	parameters_.push_back(newPara);
	return *this;
}
RootSignature& RootSignature::AddSampler(int registerNum, ShaderVisibility visibility,
	D3D12_FILTER filter, D3D12_COMPARISON_FUNC func,
	D3D12_TEXTURE_ADDRESS_MODE addressU, D3D12_TEXTURE_ADDRESS_MODE addressV, D3D12_TEXTURE_ADDRESS_MODE addressW) {
	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC newSampler{};
	newSampler.Filter = filter;
	newSampler.AddressU = addressU;
	newSampler.AddressV = addressV;
	newSampler.AddressW = addressW;
	newSampler.ComparisonFunc = func;
	newSampler.MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	newSampler.ShaderRegister = registerNum;
	newSampler.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility); // 基本的にPixelShaderで使う
	// 登録
	samplers_.push_back(newSampler);
	return *this;
}
void RootSignature::Build(ID3D12Device* device) {
	HRESULT hr = S_FALSE;

	// 詳細をセット
	desc_.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	// RootSignatureにrootParametersを登録
	desc_.pParameters = parameters_.data();		// ルートパラメータ配列へのポインタ
	desc_.NumParameters = static_cast<UINT>(parameters_.size());	// 配列の長さ
	// RootSignatureにサンプラーを登録
	desc_.pStaticSamplers = samplers_.data();
	desc_.NumStaticSamplers = static_cast<UINT>(samplers_.size());

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&desc_, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&root_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}