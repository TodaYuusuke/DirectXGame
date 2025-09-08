#include "IPlane.h"
#include "../../PrimitiveManager.h"

namespace LWP::Primitive { 
    template<typename UpdatePolicy>
    SpriteTemplate<UpdatePolicy>::SpriteTemplate() {
        this->name = ISprite::GetName() + "_" + GetNameImpl();
        LWP::Primitive::Manager::GetInstance()->SetPlanePtr(this);
        Init();
    }

    template<typename UpdatePolicy>
    SpriteTemplate<UpdatePolicy>::~SpriteTemplate() {
        LWP::Primitive::Manager::GetInstance()->DeletePlanePtr(this);
    }

    // 明示的インスタンス化
    template class SpriteTemplate<NormalPolicy>;
    template class SpriteTemplate<SequencePolicy>;
    template class SpriteTemplate<ClipPolicy>;
}