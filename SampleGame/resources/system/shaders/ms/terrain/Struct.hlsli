struct CommonData {
    uint32_t instanceSize;
};
struct InstanceData {
    float32_t3 position;
};
struct Camera {
    float32_t4x4 viewProjection; // !< ViewProjection
    float32_t4x4 rotate; // !< RotateMatrix for Billboard
    float32_t3 position; // !< WorldPosition
};

struct PayLoad {
    uint32_t groupID;
};
struct VSOutput {
    float32_t4 position : SV_POSITION0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
};
