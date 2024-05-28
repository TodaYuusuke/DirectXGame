#pragma once
#include "base/directX/utility/Command.h"
#include "renderer/NormalRenderer.h"
#include "renderer/ShadowRenderer.h"
#include "renderer/PPRenderer.h"
#include "renderer/CopyRenderer.h"
#include "renderer/MeshRenderer.h"
#include "renderer/BufferGroup.h"

#include "object/core/Particle.h"
#include "resources/texture/Texture.h"

// 前方宣言
namespace LWP::Object {
	class DirectionLight;
	class PointLight;
}
namespace LWP::Primitive {
	class IPrimitive;
	class Billboard2D;
	class Billboard3D;
	class Sprite;
}
namespace LWP::Resource {
	class ModelData;
	class RigidModel;
}


namespace LWP::Base {
	/// <summary>
	/// レンダラー管理クラス
	/// </summary>
	class RendererManager {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ 
		/// </summary>
		RendererManager() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~RendererManager() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, DXC* dxc, SRV* srv);

		/// <summary>
		/// レンダリング開始
		/// </summary>
		void DrawCall();

		/// <summary>
		/// Primitiveのデータをセット
		/// </summary>
		void AddPrimitiveData(Primitive::IPrimitive* primitive);
		
		/// <summary>
		/// 平行光源のデータをセット
		/// </summary>
		void AddLightData(Object::DirectionLight* light);
		/// <summary>
		/// 点光源のデータをセット
		/// </summary>
		void AddLightData(Object::PointLight* light);

		/// <summary>
		/// コマンドはそとで使いたいのでpointerを返す
		/// </summary>
		Command* GetCommand() { return &commander_; }


	private: // ** メンバ変数 ** //

		// コマンド管理クラス
		Command commander_;
		// バッファーまとめクラス
		BufferGroup buffers_;
		// DirectXコンパイラのポインタ
		DXC* dxc_;
		// SRVポインタ
		SRV* srv_;

		// テクスチャがないときのデフォルトテクスチャ
		Resource::Texture defaultTexture_;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// PrimitiveからIndexInfoStructに加工する関数
		/// </summary>
		/// <returns></returns>
		IndexInfoStruct ProcessIndexInfo(Primitive::IPrimitive* primitive);
		/// <summary>
		/// PrimitiveからIndexInfoStructに加工する関数
		/// </summary>
		/// <returns></returns>
		std::function<void(const IndexInfoStruct&)> ProcessSendFunction(Primitive::IPrimitive* primitive);


#pragma region 通常描画
	private:
		// レンダラー
		NormalRenderer normalRender_;
		// メッシュシェーダー
		MeshRenderer meshRenderer_;
	public:
		/// <summary>
		/// ターゲットセット
		/// </summary>
		/// <param name="view">カメラデータのView</param>
		/// <param name="back">BackBuffer（RenderRsource）</param>
		/// <param name="depth">DepthStencil</param>
		void AddTarget(const D3D12_GPU_VIRTUAL_ADDRESS& view, BackBuffer* back, DepthStencil* depth) { 
			normalRender_.AddTarget({ view, back, depth });
			meshRenderer_.AddTarget({ view, back, depth });
		}
#pragma endregion

#pragma region シャドウマッピング
	private:
		// レンダラー
		ShadowRenderer shadowRender_;
	public:
		/// <summary>
		/// ターゲットセット
		/// </summary>
		/// <param name="view">カメラデータのView</param>
		/// <param name="sm">シャドウマップリソース</param>
		void AddTarget(const D3D12_GPU_VIRTUAL_ADDRESS& view, SM_Direction* sm) { shadowRender_.AddTarget({ view, sm }); }
		void AddTarget(const std::array<D3D12_GPU_VIRTUAL_ADDRESS, 6>& views, SM_Point* sm) { shadowRender_.AddTarget({ views, sm }); }
#pragma endregion

#pragma region ポストプロセス描画
	private:
		// レンダラー
		PPRenderer ppRender_;
	public:
		/// <summary>
		/// ターゲットセット
		/// </summary>
		/// <param name="render"></param>
		/// <param name="texture"></param>
		/// <param name="depth"></param>
		/// <param name="pp">PostProcessor</param>
		void AddTarget(RenderResource* render, BackBuffer* texture, DepthStencil* depth ,PostProcessor* pp) { ppRender_.AddTarget({render, texture, depth, pp}); }
#pragma endregion

#pragma region コピー
	private:
		// レンダラー
		CopyRenderer copyRenderer_;
	public:
		/// <summary>
		/// リソースのコピー処理をセット
		/// </summary>
		/// <param name="src">コピー元のリソース</param>
		/// <param name="dst">コピー先のリソース</param>
		void AddCopyTask(RenderResource* src, RenderResource* dst) { copyRenderer_.AddTarget({src, dst}); }
#pragma endregion
	};
}