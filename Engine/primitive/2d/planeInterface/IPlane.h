#pragma once
#include "../../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// Sprite & Billboardの共通インターフェース
	/// </summary>
	class IPlane : public IPrimitive {
	public: // ** パブリックなメンバ変数 ** //

		/// <summary>
		/// 4頂点を操作しやすくするための列挙子
		/// </summary>
		enum QuadCorner {
			TopLeft = 0,		// 左上
			TopRight = 1,		// 右上
			BottomRight = 2,	// 右下
			BottomLeft = 3		// 左下
		};
		enum PlaneType {
			Sprite,
			Billboard3D,
		};

		/// <summary>
		/// 頂点
		/// <para>0 ... 左上</para>
		/// <para>1 ... 右上</para>
		/// <para>2 ... 右下</para>
		/// <para>3 ... 左下</para>
		/// </summary>
		Vertex vertices[4];

		/// <summary>
		/// アンカーポイント
		/// <para>range : 0.0f ~ 1.0f</para>
		/// <para>0なら左上、1なら右下</para>
		/// </summary>
		Math::Vector2 anchorPoint = LWP::Math::Vector2{ 0.0f,0.0f };

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IPlane();
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPlane() = default;

		/// <summary>
		/// 初期化（ユーザー呼び出し不要）
		/// </summary>
		virtual void Init() override;
		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		virtual void Update();

		/// <summary>
		/// 描画するテクスチャを読み込む関数（resources/texture直下からのパス）
		/// </summary>
		/// <param name="fileName"></param>
		virtual void LoadTexture(const std::string& fileName);
		/// <summary>
		/// テクスチャと座標を一致させる関数
		/// </summary>
		/// <param name="fileName"></param>
		virtual void FitToTexture() = 0;

		/// <summary>
		/// Debug用のImGui
		/// </summary>
		void DebugGUI() override;


	protected: // ** プロテクトなメンバ関数 ** //

		/// <summary>
		/// テクスチャと座標を一致させる関数
		/// </summary>
		/// <param name="fileName"></param>
		void FitToTexture(Math::Vector2 size);
		/// <summary>
		/// 派生クラス用のImGui
		/// </summary>
		virtual void ChildDebugGUI() {};
		/// <summary>
		/// アンカーポイントのImGui
		/// </summary>
		void ShowAnchorPointGUI();
		/// <summary>
		/// FitToSizeを呼び出すImGui
		/// </summary>
		void ShowFitToTextureGUI();


	protected: // ** プロテクトなメンバ関数 ** //

		// 形状のタイプ
		PlaneType planeType;
	};
}