#pragma once
#include "../IObject.h"

#include <functional>

#include "resources/model/RigidModel.h"
#include "base/directX/utility/RootSignature.h"
#include "base/directX/utility/PSO.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/data/RWStructuredBuffer.h"

namespace LWP::Object {
	/// <summary>
	/// GPUで処理されるパーティクル
	/// </summary>
	class GPUParticle : public IObject {
	public: // ** パブリックなメンバ変数 ** //
		// 構造体
		struct Data {
			Math::Vector3 translate = { 0.0f,0.0f,0.0f };
			Math::Vector3 scale = { 0.0f,0.0f,0.0f };
			Math::Vector3 velocity = { 0.0f,0.0f,0.0f };
			float lifeTime = 0;
		};
		struct EmitterSphere {
			Math::Vector3 position;	// 位置
			float radius;	// 射出半径
			uint32_t count;	// 射出数
			float frequency;	// 射出間隔
			float frequencyTime;	// 射出間隔調整用時間
			uint32_t emit;	// 射出許可
		};

		// 基準となるモデルクラス
		Resource::RigidModel model;


	public: // ** メンバ関数 ** //

		// コンストラクタ
		GPUParticle();

		// 初期化
		void Initialize() override final;
		// 更新
		void Update(Base::RendererManager* manager) override final;

		// パーティクルを追加
		void Add(int value);
		// パーティクルを追加（座標指定）
		void Add(int value, LWP::Math::Vector3 position);
		// シェーダーを設定
		void SetShaderPath(std::string emitter, std::string update);

		// デバッグ用GUI
		void DebugGUI() override final;

		// エミッターのフラグをfalseに
		void SetResetEmitterFlag() { emitterSphere_.data_->emit = false; }

		// レンダラー用のGetter
		Base::RootSignature* GetRoot() { return &root_; }
		Base::PSO* GetEmitterPSO() { return &emitterPSO_; }
		Base::PSO* GetUpdatePSO() { return &updatePSO_; }
		D3D12_GPU_VIRTUAL_ADDRESS GetEmitterView() { return emitterSphere_.GetGPUView(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetCountView() { return count_.GetGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetUAVDataView() { return data_.GetUAVGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVDataView() { return data_.GetSRVGPUView(); }

		/// <summary>
		/// リソースバリアを貼らせる処理
		/// </summary>
		void SetResourceBarrier(ID3D12GraphicsCommandList* list) { data_.SetResourceBarrier(list); }


	private: // ** メンバ変数 ** //
		
		// ルートシグネチャ
		Base::RootSignature root_;
		// PSO
		Base::PSO emitterPSO_;
		Base::PSO updatePSO_;

		// エミッター
		Base::ConstantBuffer<EmitterSphere> emitterSphere_;
		// パーティクルのデータ
		Base::RWStructuredBuffer<Data> data_;
		// パーティクルの数
		Base::ConstantBuffer<uint32_t> count_;

		// ディレクトリパス
		const std::string kDirectoryPath = "resources/shaders/particle/";
	};
}