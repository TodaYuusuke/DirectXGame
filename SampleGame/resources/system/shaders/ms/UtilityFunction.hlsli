
float32_t3 RGBtoHSV(float32_t3 rgb) {
    float32_t max = rgb.r > rgb.g ? rgb.r : rgb.g;
    max = max > rgb.b ? max : rgb.b;
    float32_t min = rgb.r < rgb.g ? rgb.r : rgb.g;
    min = min < rgb.b ? min : rgb.b;
    
    float32_t h = max - min;
    if (h > 0.0f)
    {
        if (max == rgb.r)
        {
            h = (rgb.g - rgb.b) / h;
            if (h < 0.0f)
            {
                h += 6.0f;
            }
        }
        else if (max == rgb.g)
        {
            h = 2.0f + (rgb.b - rgb.r) / h;
        }
        else
        {
            h = 4.0f + (rgb.r - rgb.g) / h;
        }
    }
    h /= 6.0f;
    
    float32_t s = (max - min);
    if (max != 0.0f)
        s /= max;
    
    float32_t v = max;
    
    return float32_t3(h, s, v);
}

float32_t3 HSVtoRGB(float32_t3 hsv) {
    // h(r), s(g) v(b) 
    float32_t r = hsv.b;
    float32_t g = hsv.b;
    float32_t b = hsv.b;
    if (hsv.g > 0.0f) {
        float32_t h = hsv.r * 6.0f;
        const int32_t i = int32_t(h);
        const float32_t f = h - float32_t(i);
        switch (i) {
            default:
            case 0:
                g *= 1.0f - hsv.g * (1.0f - f);
                b *= 1.0f - hsv.g;
                break;
            case 1:
                r *= 1.0f - hsv.g * f;
                b *= 1.0f - hsv.g;
                break;
            case 2:
                r *= 1.0f - hsv.g;
                b *= 1.0f - hsv.g * (1.0f - f);
                break;
            case 3:
                r *= 1.0f - hsv.g;
                g *= 1.0f - hsv.g * f;
                break;
            case 4:
                r *= 1.0f - hsv.g * (1.0f - f);
                g *= 1.0f - hsv.g;
                break;
            case 5:
                g *= 1.0f - hsv.g;
                b *= 1.0f - hsv.g * f;
                break;
        }
    }
    
    return float32_t3(r, g, b);
}

class RandomGenerator {
    float32_t3 seed;
    float32_t3 Generate3d() {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t2 Generate2d()
    {
        float32_t2 result = rand3dTo2d(seed);
        seed.x = result.x;
        seed.y = result.y;
        return result;
    }
    float32_t Generate1d() {
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
    
    
    //to 1d functions

    //get a scalar random value from a 3d value
    float rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719)) {
        //make value smaller to avoid artefacts
        float3 smallValue = sin(value);
        //get scalar value from 3d vector
        float random = dot(smallValue, dotDir);
        //make value more random by making it bigger and then taking the factional part
        random = frac(sin(random) * 143758.5453);
        return random;
    }
    float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233)) {
        float2 smallValue = sin(value);
        float random = dot(smallValue, dotDir);
        random = frac(sin(random) * 143758.5453);
        return random;
    }
    float rand1dTo1d(float3 value, float mutator = 0.546) {
        float random = frac(sin(value.x + mutator) * 143758.5453);
        return random;
    }

    //to 2d functions
    float2 rand3dTo2d(float3 value) {
        return float2(
            rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
            rand3dTo1d(value, float3(39.346, 11.135, 83.155))
        );
    }

    float2 rand2dTo2d(float2 value) {
        return float2(
            rand2dTo1d(value, float2(12.989, 78.233)),
            rand2dTo1d(value, float2(39.346, 11.135))
        );
    }

    float2 rand1dTo2d(float value) {
        return float2(
            rand2dTo1d(value, 3.9812),
            rand2dTo1d(value, 7.1536)
        );
    }

//to 3d functions

    float3 rand3dTo3d(float3 value) {
        return float3(
            rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
            rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
            rand3dTo1d(value, float3(73.156, 52.235, 09.151))
        );
    }

    float3 rand2dTo3d(float2 value) {
        return float3(
            rand2dTo1d(value, float2(12.989, 78.233)),
            rand2dTo1d(value, float2(39.346, 11.135)),
            rand2dTo1d(value, float2(73.156, 52.235))
        );
    }

    float3 rand1dTo3d(float value) {
        return float3(
            rand1dTo1d(value, 3.9812),
            rand1dTo1d(value, 7.1536),
            rand1dTo1d(value, 5.7241)
        );
    }
};