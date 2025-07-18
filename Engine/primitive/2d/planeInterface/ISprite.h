#pragma once
#include "IPlane.h"
#include "../policy/NormalPolicy.h"
#include "../policy/SequencePolicy.h"
#include "../policy/ClipPolicy.h"

namespace LWP::Primitive {
    // 前方宣言
    class Manager;

	// タグ用
	struct ISprite : virtual IPlane {
        std::string GetName() { return "Sprite"; }
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

        SpriteTemplate() {
            this->name = ISprite::GetName() + "_" + GetNameImpl();
            Primitive::Manager::GetInstance()->SetSpritePtr(this);
            Init();
        }
        ~SpriteTemplate() {
            Primitive::Manager::GetInstance()->DeleteSpritePtr(this);
        }

        void Init() override {
            ISprite::Init();
            InitImpl(&material);
            this->anchorPoint = { 0.5f, 0.5f }; // デフォルトのアンカーポイントを中央に設定
        }
        void Update() override {
            ISprite::Update();
            UpdateImpl(this->vertices, &material);
        }
        void ChildDebugGUI() override { DebugGUIImpl(); }
        void FitToTexture() { FitToTextureSprite(GetFitSizeImpl(&material)); }
    };
}