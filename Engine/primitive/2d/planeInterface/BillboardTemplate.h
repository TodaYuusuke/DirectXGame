#pragma once
#include "IPlane.h"
#include "../policy/NormalPolicy.h"
#include "../policy/SequencePolicy.h"
#include "../policy/ClipPolicy.h"

namespace LWP::Primitive {
    // 前方宣言
    class Manager;

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
            Primitive::Manager::GetInstance()->SetPlanePtr(this);
            Init();
        }
        ~BillboardTemplate() {
            Primitive::Manager::GetInstance()->SetPlanePtr(this);
        }

        void Init() override {
            RenderTag::Init();
            InitImpl(&this->material);
        }
        void Update() override {
            RenderTag::Update();
            UpdateImpl(this->vertices, &this->material);
        }
        void ChildDebugGUI() override {
            RenderTag::ChildDebugGUI();
            DebugGUIImpl();
        }
        void FitToTexture() { RenderTag::FitToTextureBillboard(GetFitSizeImpl(&this->material)); }
    };
}