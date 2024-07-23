
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