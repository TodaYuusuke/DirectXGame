#pragma once
#include "IPlane.h"
#include "../policy/NormalPolicy.h"
#include "../policy/SequencePolicy.h"
#include "../policy/ClipPolicy.h"


namespace LWP::Primitive {
    // タグ用
    struct ISprite : virtual IPlane {
        std::string GetName() override { return "Sprite"; }
        IPlane::Type GetType() override { return Type::Sprite; }
    };


    template<typename UpdatePolicy>
    class SpriteTemplate : public ISprite, public UpdatePolicy {
    public:
        using UpdatePolicy::InitImpl;
        using UpdatePolicy::UpdateImpl;
        using UpdatePolicy::DebugGUIImpl;
        using UpdatePolicy::GetFitSizeImpl;
        using UpdatePolicy::GetNameImpl;
        //using UpdatePolicy::PostLoadTextureImpl;

        SpriteTemplate();
        ~SpriteTemplate();

        void Init() override {
            ISprite::Init();
            InitImpl(&material);
        }
        void Update() override {
            ISprite::Update();
            UpdateImpl(this->vertices, &material);
        }
        void ChildDebugGUI() override {
            ISprite::ChildDebugGUI();
            DebugGUIImpl();
        }
        void FitToTexture() { FitToTextureSprite(GetFitSizeImpl(&material)); }
    };


    using NormalSprite = SpriteTemplate<NormalPolicy>;
    using SequenceSprite = SpriteTemplate<SequencePolicy>;
    using ClipSprite = SpriteTemplate<ClipPolicy>;
}