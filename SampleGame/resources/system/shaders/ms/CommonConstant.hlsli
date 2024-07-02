const static uint32_t kMaxSize_Texture = 500;
const static uint32_t kMaxSize_DirSM = 1;
const static uint32_t kMaxSize_PointSM = 8;

const static uint32_t kOffset_Texture = 0;
const static uint32_t kOffset_DirSM = kMaxSize_Texture;
const static uint32_t kOffset_PointSM = kMaxSize_Texture + kMaxSize_DirSM;
    
#define REGISTER_OFFSET(start, offset) (start + offset)

const static float kShadowDensity = 0.2f;