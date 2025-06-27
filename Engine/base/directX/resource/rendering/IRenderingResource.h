#pragma once
#include "../IResource.h"

#include "Config.h"

namespace LWP::Base {
	/// <summary>
	/// レンダリング用のリソース基底クラス
	/// </summary>
	class IRenderingResource
		: public IResource {
	public: // ** メンバ関数 ** //

		// 初期化
		virtual void Init() = 0;
		// 画面クリア
		virtual void Clear(ID3D12GraphicsCommandList* list) = 0;

		/// <summary>
		/// 現在のバリアを取得
		/// </summary>
		/// <returns></returns>
		D3D12_RESOURCE_STATES GetBarrier() { return currentBarrierState; }
		/// <summary>
		/// リソースバリアを変更
		/// </summary>
		/// <param name="state"></param>
		/// <param name="list"></param>
		void ChangeResourceBarrier(D3D12_RESOURCE_STATES state, ID3D12GraphicsCommandList* list) {
			// 変更するバリアと今のバリアが同じ場合は何もしない
			if (currentBarrierState == state) { return; }

			// previous を保存しておく
			previousBarrierState = currentBarrierState;

			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// 全てのサブリソースを選択
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			// バリアを張る対象のリソース
			barrier.Transition.pResource = resource_.Get();
			// 遷移前（現在）のResourceState
			barrier.Transition.StateBefore = currentBarrierState;
			// 遷移後のResourceState
			barrier.Transition.StateAfter = state;

			// リソースバリア変更
			list->ResourceBarrier(1, &barrier);
			// 現在のステータスを変更しておく
			currentBarrierState = state;
		}
		/// <summary>
		/// バリアを1つ前の状態に戻す関数
		/// </summary>
		/// <param name="list"></param>
		void RevertResourceBarrier(ID3D12GraphicsCommandList* list) {
			// 不正な状態（まだ一度もバリアを変更していない場合）ならばエラー
			//assert(previousBarrierState != InvalidState && "RevertResourceBarrier() called before any ChangeResourceBarrier() call.");
			if (previousBarrierState == InvalidState) { return; }
			// 変更するバリアと今のバリアが同じ場合は何もしない
			if (currentBarrierState == previousBarrierState) { return; }

			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// 全てのサブリソースを選択
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			// バリアを張る対象のリソース
			barrier.Transition.pResource = resource_.Get();
			// 遷移前（現在）のResourceState
			barrier.Transition.StateBefore = currentBarrierState;
			// 遷移後のResourceState
			barrier.Transition.StateAfter = previousBarrierState;

			// リソースバリア変更
			list->ResourceBarrier(1, &barrier);
			// 現在のステータスと前のステータスを交換しておく
			D3D12_RESOURCE_STATES tmp = currentBarrierState;
			currentBarrierState = previousBarrierState;
			previousBarrierState = tmp;
		}

		/// <summary>
		/// リソースのビューポートを返す関数
		/// </summary>
		/// <returns></returns>
		D3D12_VIEWPORT GetViewport() {
			D3D12_VIEWPORT viewport = {};	// ビューポート
			// クライアント領域のサイズと一緒にして画面全体に表示
			viewport.Width = static_cast<float>(width);
			viewport.Height = static_cast<float>(height);
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			return viewport;
		}
		/// <summary>
		/// リソースのシザー矩形を返す関数
		/// </summary>
		/// <returns></returns>
		D3D12_RECT GetScissorRect() {
			D3D12_RECT scissorRect = {};
			// 基本的にビューポートと同じ矩形が構成されるようにする
			scissorRect.left = 0;
			scissorRect.right = width;
			scissorRect.top = 0;
			scissorRect.bottom = height;
			return scissorRect;
		}
		/// <summary>
		/// Mipレベル数を返す 
		/// </summary>
		/// <returns></returns>
		UINT16 GetMipLevels() const { return desc.MipLevels; }

	public: // ** パブリックなメンバ変数 ** //
		
		// 解像度
		int width = Config::Window::kResolutionWidth;
		int height = Config::Window::kResolutionHeight;

		// Resourceの設定
		D3D12_RESOURCE_DESC desc;
		// クリアカラー（Heapに登録後に変更しないでください）
		D3D12_CLEAR_VALUE clearValue;
		// heapの設定
		D3D12_HEAP_PROPERTIES properties;

	protected: // ** プライベートなメンバ変数 ** //

		const D3D12_RESOURCE_STATES InvalidState = static_cast<D3D12_RESOURCE_STATES>(0xFFFFFFFF);	// 無効な状態

		// 現在のリソースバリア
		D3D12_RESOURCE_STATES currentBarrierState;
		// 前回のリソースバリア
		D3D12_RESOURCE_STATES previousBarrierState = InvalidState;

	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}