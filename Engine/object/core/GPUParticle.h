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
			Math::Vector4 color;
			float lifeTime = 0;
			uint32_t isActive;	// 生存フラグ
		};
		struct EmitterSphere {
			Math::Vector3 position;	// 位置
			float radius;	// 射出半径
			float frequency;	// 射出間隔
			float frequencyTime;	// 射出間隔調整用時間
		};

		// 基準となるモデルクラス
		Resource::RigidModel model;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		GPUParticle();
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="capacityMultiply">パーティクル最大量の倍率</param>
		GPUParticle(int capacityMultiply);

		// 初期化
		void Initialize() override final;
		// 更新
		void Update(Base::RendererManager* manager) override final;

		// パーティクルを追加
		void Add(uint32_t value);
		// パーティクルを追加（座標指定）
		void Add(uint32_t value, LWP::Math::Vector3 position);
		// シェーダーを設定
		void SetShaderPath(std::string emitter, std::string update);

		// デバッグ用GUI
		void DebugGUI() override final;

		// レンダラー用のGetter
		Base::RootSignature* GetRoot() { return &root_; }
		Base::PSO* GetEmitterPSO() { return &emitterPSO_; }
		Base::PSO* GetUpdatePSO() { return &updatePSO_; }
		D3D12_GPU_VIRTUAL_ADDRESS GetEmitterView() { return emitterSphere_.GetGPUView(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetCountView() { return count_.GetGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetUAVDataView() { return data_.GetUAVGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVDataView() { return data_.GetSRVGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListIndexView() { return freeListIndex_.GetUAVGPUView(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListView() { return freeList_.GetUAVGPUView(); }

		int GetMultiply() { return multiply; }
		int GetEmitCount() { return emitCount; }
		bool GetIsInit() { return isInit; }
		void ClearIsInit() { isInit = false; }
		bool GetIsEmit() { return isEmit; }
		void ClearIsEmit() { isEmit = false; }

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
		// パーティクルの未使用インデックスカウンター
		Base::RWStructuredBuffer<int> freeListIndex_;
		Base::RWStructuredBuffer<uint32_t> freeList_;
		// パーティクルの最大数
		Base::ConstantBuffer<uint32_t> count_;

		// パーティクルの最大数倍率
		int multiply = 1;
		// 初期化命令を出すフラグ
		bool isInit = true;
		// 生成命令を出すフラグ
		bool isEmit = false;
		// 生成する数
		int emitCount = 0;

		// ディレクトリパス
		const std::string kDirectoryPath = "resources/shaders/particle/";
	};
}