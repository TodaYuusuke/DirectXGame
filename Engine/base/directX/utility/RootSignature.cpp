#include "RootSignature.h"
#include "utility/MyUtility.h"

using namespace LWP::Base;

void RootSignature::Init() {
	desc_ = D3D12_ROOT_SIGNATURE_DESC();
	parameters_.clear();
	parametersDesc_.clear();
	samplers_.clear();
	defaultTableDesc_ = {
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// TableなのでSRVを使う
		1,	// 数は1つ
		0,	// ここは可変
		0,	// スペースは基本0
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND	// Offsetを自動計算
	};
}

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
	D3D12_DESCRIPTOR_RANGE newDesc = defaultTableDesc_;
	newDesc.BaseShaderRegister = registerNum;	// レジスタ番号を登録
	newDesc.RegisterSpace = space;	// スペースを設定（デフォは0）
	if (maxSize != 0) { newDesc.NumDescriptors = maxSize; }	// 0じゃないときのみ最大数を登録
	// 設定も登録
	parametersDesc_.push_back(newDesc);

	// 新しく登録するデータ
	D3D12_ROOT_PARAMETER newPara{};
	newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	newPara.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility);
	newPara.DescriptorTable.pDescriptorRanges = &parametersDesc_.back(); // Tabelの中身の配列を指定
	newPara.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数
	// 登録
	parameters_.push_back(newPara);
	return *this;
}
RootSignature& RootSignature::AddUAVParameter(int registerNum, ShaderVisibility visibility, int space, UINT maxSize) {
	// 新しく登録するTableの設定
	D3D12_DESCRIPTOR_RANGE newDesc = defaultTableDesc_;
	newDesc.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	newDesc.BaseShaderRegister = registerNum;	// レジスタ番号を登録
	newDesc.RegisterSpace = space;	// スペースを設定（デフォは0）
	if (maxSize != 0) { newDesc.NumDescriptors = maxSize; }	// 0じゃないときのみ最大数を登録
	// 設定も登録
	parametersDesc_.push_back(newDesc);

	// 新しく登録するデータ
	D3D12_ROOT_PARAMETER newPara{};
	//newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	newPara.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility);
	newPara.DescriptorTable.pDescriptorRanges = &parametersDesc_.back(); // Tabelの中身の配列を指定
	newPara.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数
	// 登録
	parameters_.push_back(newPara);
	return *this;
}
RootSignature& RootSignature::AddUAVRWTexture(int registerNum, ShaderVisibility visibility, int space) {
	// 新しく登録するTableの設定
	D3D12_DESCRIPTOR_RANGE newDesc = defaultTableDesc_;
	newDesc.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	newDesc.BaseShaderRegister = registerNum;	// レジスタ番号を登録
	newDesc.RegisterSpace = space;	// スペースを設定（デフォは0）
	newDesc.NumDescriptors = 0;
	// 設定も登録
	parametersDesc_.push_back(newDesc);

	// 新しく登録するデータ
	D3D12_ROOT_PARAMETER newPara{};
	newPara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	newPara.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility);
	newPara.DescriptorTable.pDescriptorRanges = &parametersDesc_.back(); // Tabelの中身の配列を指定
	newPara.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数
	// 登録
	parameters_.push_back(newPara);
	return *this;
}


RootSignature& RootSignature::AddSampler(int registerNum, ShaderVisibility visibility,
	D3D12_FILTER filter, D3D12_COMPARISON_FUNC func, D3D12_TEXTURE_ADDRESS_MODE address,
	D3D12_STATIC_BORDER_COLOR borderColor, int maxAnisotropy) {
	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC newSampler{};
	newSampler.Filter = filter;
	newSampler.AddressU = address;
	newSampler.AddressV = address;
	newSampler.AddressW = address;
	newSampler.ComparisonFunc = func;
	newSampler.BorderColor = borderColor;
	newSampler.MaxAnisotropy = maxAnisotropy;
	newSampler.MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	newSampler.ShaderRegister = registerNum;
	newSampler.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(visibility); // 基本的にPixelShaderで使う
	// 登録
	samplers_.push_back(newSampler);
	return *this;
}
void RootSignature::Build() {
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
	hr = GPUDevice::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&root_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}

RootSignature& RootSignature::Copy(const RootSignature& source) {
	desc_ = source.desc_;	// 詳細設定をコピー
	// パラメータのコピー用
	source.CopyParameters(
		&parameters_,
		&parametersDesc_,
		&samplers_
	);


	return *this;
}

void RootSignature::CopyParameters(
	std::vector<D3D12_ROOT_PARAMETER>* para,
	std::list<D3D12_DESCRIPTOR_RANGE>* paraDesc,
	std::vector<D3D12_STATIC_SAMPLER_DESC>* samplers
) const {
	// パラメータのコピー
	para->clear();
	para->resize(parameters_.size());
	std::copy(parameters_.begin(), parameters_.end(), para->begin());
	// パラメータのDescのコピー
	paraDesc->clear();
	paraDesc->resize(parametersDesc_.size());
	std::copy(parametersDesc_.begin(), parametersDesc_.end(), paraDesc->begin());
	// サンプラーのコピー
	samplers->clear();
	samplers->resize(samplers_.size());
	std::copy(samplers_.begin(), samplers_.end(), samplers->begin());

	// Descのポインタを再設定
	std::list<D3D12_DESCRIPTOR_RANGE>::iterator it = paraDesc->begin();
	for (int i = 0; i < para->size(); i++) {
		D3D12_ROOT_PARAMETER& p = (*para)[i];	// 参照で取得
		if (p.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV) { continue; }	// CBVなら登録するものがないので無視
		p.DescriptorTable.pDescriptorRanges = &(*it); // Tabelの中身の配列を指定
	}
}