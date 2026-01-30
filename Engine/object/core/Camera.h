#pragma once
#include "../IObject.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/rendering/gBuffer/GBuffer.h"

#include "base/directX/postProcess/PostProcessor.h"

#include "resources/texture/Texture.h"


// データ構造体のためだけに前方宣言しなきゃいけない
namespace LWP::Object { class Camera; }
// カメラ構造体
namespace LWP::Base {
	struct CameraStruct {
		Math::Matrix4x4 viewProjection;
		Math::Matrix4x4 vp2D;
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
		// ポストプロセス
		Base::PostProcessor pp;

		// -- ポストエフェクトフラグ -- //

		// シェーダー用のパス（作成後は意味をなくすのでいつか再設計）
		std::string shaderPath = "postProcess/CCTV.PS.hlsl";


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Camera();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Camera();

		// 初期化
		void Init() override;
		// 更新
		void Update() override;

		/// <summary>
		/// このカメラをメインの描画に使用する
		/// </summary>
		void SetMainCamera();

		// デバッグ用GUI
		void DebugGUI() override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjection() const;
		// レンダリング結果のテクスチャのポインタを受け取る関数
		Resource::Texture GetTexture() { return textureResource_; }
		
		// GBufferを返す関数
		Base::GBuffer* GetGBuffer() { return &gBuffer_; }
		// レンダリング結果を書き込む用のリソースを取得する関数
		Base::RenderResource* GetTextureResource() { return &textureResource_; }
		/// <summary>
		/// カメラのデータのViewを返す関数
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetBufferView() { return constantBuffer_.GetGPUView(); }


	private: // ** メンバ変数 ** //

		// レンダリング先のリソースと参照するリソースを別で分け、レンダリング中に参照する場合に備える。

		// このカメラでレンダリングするためのデータ
		Base::ConstantBuffer<Base::CameraStruct> constantBuffer_;

		// 書き込むリソース
		Base::GBuffer gBuffer_;
		//																																												レンダリング結果の画像
		Base::RenderResource textureResource_;
		// 結果の画像
		//Base::RenderResource textureResource_;
	};
}
