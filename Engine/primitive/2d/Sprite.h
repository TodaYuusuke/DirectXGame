#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// テクスチャを描画するための形状
	/// </summary>
	class Sprite final {
	public: // ** パブリックなメンバ関数 ** //

		// テクスチャ
		Resource::Texture texture;
		// アンカーポイント
		Math::Vector2 anchorPoint = Math::Vector2{ 0.0f,0.0f };


	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sprite();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sprite();

		/// <summary>
		/// スプライトとして描画命令
		/// </summary>
		/// <param name="pos">描画するスクリーン座標</param>
		/// <param name="rotateZ">Z軸回転</param>
		/// <param name="scale">拡大率</param>
		/// <param name="color">色</param>
		void Draw(Math::Vector2 pos, float rotateZ = 0.0f, Math::Vector2 scale = { 1.0f,1.0f }, Utility::Color color = Utility::ColorPattern::WHITE);
		/// <summary>
		/// Billboard2Dとして描画命令
		/// </summary>
		/// <param name="pos">描画するワールド座標</param>
		/// <param name="rotate">回転</param>
		/// <param name="scale">拡大率</param>
		/// <param name="color">色</param>
		void DrawBillboard2D(Math::Vector3 pos, Math::Vector3 rotate = { 1.0f,1.0f,1.0f }, Math::Vector2 scale = { 1.0f,1.0f }, Utility::Color color = Utility::ColorPattern::WHITE);
		/// <summary>
		/// Billboard3Dとして描画命令
		/// </summary>
		/// <param name="pos">描画するワールド座標</param>
		/// <param name="rotate">回転</param>
		/// <param name="scale">拡大率</param>
		/// <param name="color">色</param>
		void DrawBillboard3D(Math::Vector3 pos, Math::Vector3 rotate = { 1.0f,1.0f,1.0f }, Math::Vector2 scale = { 1.0f,1.0f }, Utility::Color color = Utility::ColorPattern::WHITE);

		/// <summary>
		/// Debug用のImGui
		/// </summary>
		/// <param name="label"></param>
		void DebugGUI(const std::string& label = "Sprite");


	private: // ** メンバ変数 ** //

		// レンダリングマネージャーのポインタ
		Base::RendererManager* manager_ = nullptr;
	};
}