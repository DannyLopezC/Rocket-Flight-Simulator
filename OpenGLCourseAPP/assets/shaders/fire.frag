#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform float uTime;

float hash(vec2 p)
{
    p = fract(p * vec2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return fract(p.x * p.y);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main()
{
    vec2 uv = vUV;

    float x = uv.x - 0.5;
    float y = uv.y;

    // Distorsión leve
    float n = noise(vec2(x * 4.0, y * 6.0 - uTime * 3.0));
    x += (n - 0.5) * 0.05;

    // Forma de llama:
    // angosta al inicio, más ancha al medio, y termina en punta
    float width =
        0.015 +
        0.14 * sin(y * 3.14159);

    // ligera respiración
    width *= 0.97 + 0.095 * sin(uTime * 12.0 + y * 8.0);

    float flame = 1.0 - smoothstep(width, width + 0.045, abs(x));

    float coreWidth = width * 0.35;
    float core = 1.0 - smoothstep(coreWidth, coreWidth + 0.02, abs(x));

    // Fade vertical corto
    float verticalFade = smoothstep(0.02, 0.08, y) * (1.0 - smoothstep(0.82, 0.98, y));

    float intensity = flame * verticalFade;
    float coreIntensity = core * verticalFade;

    vec3 outerLow  = vec3(1.0, 0.20, 0.0);
    vec3 outerHigh = vec3(1.0, 0.55, 0.0);
    vec3 innerLow  = vec3(1.0, 0.85, 0.2);
    vec3 innerHigh = vec3(1.0, 1.0, 0.95);

    vec3 outerColor = mix(outerLow, outerHigh, y);
    vec3 innerColor = mix(innerLow, innerHigh, y);

    vec3 finalColor = outerColor * intensity + innerColor * coreIntensity * 1.15;

    float alpha = max(intensity, coreIntensity * 0.9);
    alpha = smoothstep(0.0, 0.10, alpha);

    FragColor = vec4(finalColor, alpha);
}