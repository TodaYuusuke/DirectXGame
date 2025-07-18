#pragma once
#include "IPlane.h"
#include "../policy/NormalPolicy.h"
#include "../policy/SequencePolicy.h"
#include "../policy/ClipPolicy.h"

namespace LWP::Primitive {
    // 前方宣言
    class Manager;

    // タグ用
    struct IBillboard2D : virtual IPlane {
        std::string GetName() { return "Billboard2D"; }
    };

    template<typename UpdatePolicy, typename RenderTag>
    class BillboardTemplate : public RenderTag, public UpdatePolicy {
        static_assert(std::is_base_of_v<IPlane, RenderTag>, "RenderTag must derive from IPlane");
    public:
        using UpdatePolicy::InitImpl;
        using UpdatePolicy::UpdateImpl;
        using UpdatePolicy::DebugGUIImpl;
        using UpdatePolicy::GetFitSizeImpl;
        using UpdatePolicy::GetNameImpl;
        //using UpdatePolicy::PostLoadTextureImpl;

        BillboardTemplate() {
            this->name = RenderTag::GetName() + "_" + GetNameImpl();
            Primitive::Manager::GetInstance()->SetBillboardPtr(this);
            Init();
        }
        ~BillboardTemplate() {
            Primitive::Manager::GetInstance()->DeleteBillboardPtr(this);
        }

        void Init() override {
            RenderTag::Init();
            InitImpl(&this->material);
            this->anchorPoint = { 0.5f, 0.5f }; // デフォルトのアンカーポイントを中央に設定
        }
        void Update() override {
            RenderTag::Update();
            UpdateImpl(this->vertices, &this->material);
        }
        void ChildDebugGUI() override { DebugGUIImpl(); }
        void FitToTexture() { RenderTag::FitToTextureBillboard(GetFitSizeImpl(&this->material)); }
    };
}