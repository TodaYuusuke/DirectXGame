#pragma once
#include "../IObject.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"

// データ構造体のためだけに前方宣言しなきゃいけない
namespace LWP::Object { class Camera; }
// カメラ構造体
namespace LWP::Base {
	struct CameraStruct {
		Math::Matrix4x4 viewProjection;
		Math::Matrix4x4 rotate;
		Math::Vector3 position;

		CameraStruct& operator=(const Object::Camera& value);
	};
}

namespace LWP::Object {
	/// <summary>
	/// カメラクラス
	/// </summary>
	class Camera final : public IObject {
	public: // ** パブリックなメンバ変数 ** //

		// FOV
		float fov = 90.0f;

		// -- ポストエフェクトフラグ -- //

		// ポストプロセスを行うかのフラグ
		bool isUsePostProcess = false;
		// シェーダー用のパス（作成後は意味をなくすのでいつか再設計）
		std::string shaderPath = "postProcess/CCTV.PS.hlsl";


	public: // ** メンバ関数 ** //

		// デフォルトコンストラクタ
		Camera();
		// デフォルトデストラクタ
		~Camera() = default;

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjection() const;

		// レンダリング結果のテクスチャのポインタを受け取る関数
		Resource::Texture GetTexture();
		
		/// <summary>
		/// カメラのデータのViewを返す関数
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetBufferView() { return constantBuffer_.GetGPUView(); }


	private: // ** メンバ変数 ** //

		//// このカメラでレンダリングするためのデータ
		Base::ConstantBuffer<Base::CameraStruct> constantBuffer_;
		//// レンダリング結果のリソース
		Base::RenderResource renderResource_;
		//// デプスステンシルのリソース
		Base::DepthStencil depthStencil_;

		// このカメラからのレンダリング結果を格納する変数
		//Resource::RenderTexture* renderTexture_ = nullptr;
		// ポストプロセス用レンダラー
		//Base::PostProcessRenderer* ppRenderer_ = nullptr;
	};
}
