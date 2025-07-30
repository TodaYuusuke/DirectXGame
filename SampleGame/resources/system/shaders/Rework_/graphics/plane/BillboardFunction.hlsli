#include "Plane.hlsli"

float32_t4x4 DirectionToDirection(float32_t3 from, float32_t3 to) {
    float32_t c = dot(from, to);
    
    if (c > 0.9999f) {
        return float32_t4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    
    float32_t s = length(cross(from, to));
    float32_t3 n = normalize(cross(from, to));
    n = c <= -1.0f ? float32_t3(from.y, -from.x, 0.0f) : n;

    float32_t4x4 result;
    result[0][0] = (n.x * n.x) * (1.0f - c) + c;
    result[0][1] = (n.x * n.y) * (1.0f - c) + n.z * s;
    result[0][2] = (n.x * n.z) * (1.0f - c) - n.y * s;
    result[0][3] = 0.0f;

    result[1][0] = (n.y * n.x) * (1.0f - c) - n.z * s;
    result[1][1] = (n.y * n.y) * (1.0f - c) + c;
    result[1][2] = (n.y * n.z) * (1.0f - c) + n.x * s;
    result[1][3] = 0.0f;

    result[2][0] = (n.z * n.x) * (1.0f - c) + n.y * s;
    result[2][1] = (n.z * n.y) * (1.0f - c) - n.x * s;
    result[2][2] = (n.z * n.z) * (1.0f - c) + c;
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}

float32_t4 CreateQuaternionFromAxisAngle(float32_t3 axis, float32_t radian) {
    float32_t3 normalizedAxis = normalize(axis);
    float32_t halfAngle = radian * 0.5f;

    float s = sin(halfAngle);
    float c = cos(halfAngle);

    float32_t4 result;
    result.x = normalizedAxis.x * s;
    result.y = normalizedAxis.y * s;
    result.z = normalizedAxis.z * s;
    result.w = c;

    return normalize(result);
}

float32_t3 VectorMulQuaternion(float32_t3 pos, float32_t4 q) {
    return (q.w * q.w - dot(q.xyz, q.xyz)) * pos + 2.0 * q.xyz * dot(q.xyz, pos) + 2 * q.w * cross(q.xyz, pos);
}

float32_t4 Surface(Vertex v, WorldTransform wtf) {
    return mul(mul(v.position, wtf.m), vCamera.m);
}
float32_t4 Billboard2D(Vertex v, WorldTransform wtf) {
    float32_t4x4 mat = mul(mul(wtf.scale, mul(wtf.rotate, vCamera.rotate)), wtf.translate);
    return mul(mul(v.position, mat), vCamera.m);
}
float32_t4 Billboard3D(Vertex v, WorldTransform wtf, int32_t instanceID) {
    float32_t3 center = GetWorldCenterPosition(instanceID);
    float32_t3 from = normalize(float3(0.0f, 0.0f, -1.0f));
    float32_t3 to = normalize(vCamera.worldPos - center);
    float32_t4x4 m = DirectionToDirection(from, to);
    
    return mul(mul(mul(v.position, wtf.m), m), vCamera.m);
}
float32_t4 HorizontalBillboard(Vertex v, WorldTransform wtf, int32_t instanceID) {
    float32_t3 center = GetWorldCenterPosition(instanceID);
    float32_t3 toCamera = center - vCamera.worldPos;
    
    toCamera.y = 0.0f;
    float32_t3 forward = normalize(toCamera);
    float32_t3 up = float32_t3(0.0f, 1.0f, 0.0f);
    float32_t3 right = normalize(cross(up, forward));
    forward = normalize(cross(right, up));

    float32_t4x4 billboardMatrix = float32_t4x4(
        float32_t4(right, 0.0f),
        float32_t4(up, 0.0f),
        float32_t4(forward, 0.0f),
        float32_t4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    float32_t4x4 mat = mul(mul(mul(wtf.scale, wtf.rotate), billboardMatrix), wtf.translate);
    return mul(mul(v.position, mat), vCamera.m);
}
float32_t4 VerticalBillboard(Vertex v, WorldTransform wtf, int32_t instanceID) {
    float32_t3 center = GetWorldCenterPosition(instanceID);
    float32_t3 toCamera = center - vCamera.worldPos;
    
    float32_t3 forward = normalize(float32_t3(toCamera.x, 0.0f, toCamera.z));
    float32_t3 up = float32_t3(0.0f, 0.0f, 1.0f);
    float32_t3 right = normalize(cross(up, forward));
    forward = normalize(cross(right, up));

    float32_t4x4 billboardMatrix = float32_t4x4(
        float32_t4(right, 0.0f),
        float32_t4(up, 0.0f),
        float32_t4(forward, 0.0f),
        float32_t4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    float32_t4x4 mat = mul(mul(mul(wtf.scale, wtf.rotate), billboardMatrix), wtf.translate);
    return mul(mul(v.position, mat), vCamera.m);
}

float32_t4 StretchedBillboard(Vertex v, WorldTransform wtf, float32_t3 velocity, int32_t instanceID) {
    // Culc Rotate
    float32_t3 right = float32_t3(1.0f, 0.0f, 0.0f);
    float32_t4x4 velocityMatrix = DirectionToDirection(right, normalize(velocity));
    
    float32_t3 center = GetWorldCenterPosition(instanceID);
    float32_t3 toCamera = normalize(vCamera.worldPos - center);
    float32_t3 localToCamera = mul(toCamera, (float32_t3x3) transpose(velocityMatrix));
    float32_t pitch = atan2(localToCamera.y, -localToCamera.z);
    
    // Culc Scale
    float32_t baseScaleX = 1.0f;
    float32_t stretchFactor = 0.1f;
    float32_t scaleX = baseScaleX + length(velocity) * stretchFactor;

    float32_t4x4 stretchMatrix = float32_t4x4(
        scaleX, 0.0f, 0.0f, 0.0f,
        0.0f,   1.0f, 0.0f, 0.0f,
        0.0f,   0.0f, 1.0f, 0.0f,
        0.0f,   0.0f, 0.0f, 1.0f
    );
    
    float32_t4x4 s = mul(wtf.scale, stretchMatrix);
    float32_t4x4 r = mul(wtf.rotate, velocityMatrix);
    
    float32_t4 rotated = float32_t4(
                            VectorMulQuaternion(mul(mul(v.position, s), r).xyz, CreateQuaternionFromAxisAngle(velocity, pitch)),
                            1.0f
                         );
    return mul(mul(rotated, wtf.translate), vCamera.m);

}