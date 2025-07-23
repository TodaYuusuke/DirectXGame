#pragma once
#include "planeInterface/BillboardTemplate.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面（ビルボード補正無し）
	/// </summary>
	struct ISurface : virtual IPlane {
		std::string GetName() override { return "Surface"; }
		IPlane::Type GetType() override { return Type::Surface; }
	};
	using NormalSurface = BillboardTemplate<NormalPolicy, ISurface>;
	using SequenceSurface = BillboardTemplate<SequencePolicy, ISurface>;
	using ClipSurface = BillboardTemplate<ClipPolicy, ISurface>;

	/// <summary>
	/// ビルボード2D（カメラの'面'を見る）
	/// </summary>
	struct IBillboard2D : virtual IPlane {
		std::string GetName() override { return "Billboard2D"; }
		IPlane::Type GetType() override { return Type::Billboard2D; }
	};
	using NormalBillboard2D = BillboardTemplate<NormalPolicy, IBillboard2D>;
	using SequenceBillboard2D = BillboardTemplate<SequencePolicy, IBillboard2D>;
	using ClipBillboard2D = BillboardTemplate<ClipPolicy, IBillboard2D>;

	/// <summary>
	/// 水平ビルボード（XZ平面に対して水平）
	/// </summary>
	struct IHorizontalBillboard : virtual IPlane {
		std::string GetName() override { return "HorizontalBillboard"; }
		IPlane::Type GetType() override { return Type::HorizontalBillboard; }
	};
	using NormalHorizontalBillboard = BillboardTemplate<NormalPolicy, IHorizontalBillboard>;
	using SequenceHorizontalBillboard = BillboardTemplate<SequencePolicy, IHorizontalBillboard>;
	using ClipHorizontalBillboard = BillboardTemplate<ClipPolicy, IHorizontalBillboard>;

	/// <summary>
	/// 垂直ビルボード（XZ平面に対して垂直）
	/// </summary>
	struct IVerticalBillboard : virtual IPlane {
		std::string GetName() override { return "VerticalBillboard"; }
		IPlane::Type GetType() override { return Type::VerticalBillboard; }
	};
	using NormalVerticalBillboard = BillboardTemplate<NormalPolicy, IVerticalBillboard>;
	using SequenceVerticalBillboard = BillboardTemplate<SequencePolicy, IVerticalBillboard>;
	using ClipVerticalBillboard = BillboardTemplate<ClipPolicy, IVerticalBillboard>;

	/// <summary>
	/// ストレッチ（伸縮）ビルボード
	/// </summary>
	struct StretchedBillboard : virtual IPlane {
		// 速度（描画計算用　※自動で加算されるわけではないので注意）
		Math::Vector3 velocity = { 1.0f, 0.0f, 0.0f };

		std::string GetName() override { return "StretchedBillboard"; }
		IPlane::Type GetType() override { return Type::StretchedBillboard; }
		void ChildDebugGUI() override;
	};

	using NormalStretchedBillboard = BillboardTemplate<NormalPolicy, StretchedBillboard>;
	using SequenceStretchedBillboard = BillboardTemplate<SequencePolicy, StretchedBillboard>;
	using ClipStretchedBillboard = BillboardTemplate<ClipPolicy, StretchedBillboard>;
}