#pragma once
#include "../GPUDevice.h"
#include "math/vector/Vector2.h"

namespace LWP::Base {
	// 前方宣言
	class CommandManager;

	/// <summary>
	/// レンダリングに使うリソースクラス
	/// </summary>
	class RenderResource {
	public: // ** メンバ関数 ** //

		RenderResource() = delete;
		RenderResource(ID3D12Device* device, CommandManager* manager);
		~RenderResource() = default;

		// 解像度をセットする関数
		void SetResolution(const int width, const int height) {
			width_ = width;
			height_ = height;
		}

		// RTVに登録する関数
		bool RegisterRTV();
		// RTVに再登録する関数
		bool ReRegisterRTV();
		// SRVに登録する関数
		bool RegisterSRV();
		// SRVに再登録する関数
		bool ReRegisterSRV();
		// DSVに登録する関数
		bool RegisterDSV();
		// DSVに再登録する関数
		bool ReRegisterDSV();

		// リソースバリアをセットする関数
		void SetResourceBarrier(D3D12_RESOURCE_BARRIER barrier);


	public: // ** ゲッター ** //

		// リソースを返す関数
		ID3D12Resource* GetResource() { return resource_.Get(); }

		// 現在の解像度を取得
		Math::Vector2 GetResolution() const {
			return { static_cast<float>(width_),static_cast<float>(height_) };
		};

		// RTVのインデックスを返す関数
		int GetSRV() { return srvIndex_; }
		// RTVのインデックスを返す関数
		int GetRTV() { return rtvIndex_; }
		// DSVのインデックスを返す関数
		int GetDSV() { return dsvIndex_; }

		// 現在のリソースバリアを受け取る関数
		D3D12_RESOURCE_BARRIER SetResourceBarrier() { return currentBarrier_; }


	private: // ** メンバ変数 ** //

		// リソース本体
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
		// コマンドマネージャーのポインタ
		CommandManager* manager_ = nullptr;

		// 解像度
		int width_ = -1;
		int height_ = -1;

		// SRVのインデックス
		int srvIndex_ = -1;
		// RTVのインデックス
		int rtvIndex_ = -1;
		// 深度マップのリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> depthMapResource_ = nullptr;
		// DSVのインデックス
		int dsvIndex_ = -1;

		// 現在のリソースバリア
		D3D12_RESOURCE_BARRIER currentBarrier_;


	private: // ** メンバ変数 ** //

		// リソースの実体を作成する関数
		void CreateResource(ID3D12Device* device);
	};
}