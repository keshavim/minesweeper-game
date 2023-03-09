#version 330 core
layout (location = 0) in vec4 a_vertex;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(a_vertex.xy, 0.0, 1.0);
    TexCoords = a_vertex.zw;
}  

//VERTEX_END

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform int effect;

const float offset = 1.0 / 300.0;
vec2 offsets[9] = vec2[](vec2(-offset, offset), // top-left
    vec2(0.0f, offset), // top-center
    vec2(offset, offset), // top-right
    vec2(-offset, 0.0f),   // center-left
    vec2(0.0f, 0.0f),   // center-center
    vec2(offset, 0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2(0.0f, -offset), // bottom-center
    vec2(offset, -offset)  // bottom-right    
    );

vec4 inversion() {
    return vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}
vec4 grayscale() {
    vec4 color = texture(screenTexture, TexCoords);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec4(average, average, average, 1.0);
}

vec4 kernal1() {
    
    float kernel[9] = float[](-1, -1, -1, -1, 9, -1, -1, -1, -1);

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) col += sampleTex[i] * kernel[i];

    return vec4(col, 1.0);
}
vec4 kernal2() {
    
    float kernel[9] = float[](1, 1, 1, 1, 8, 1, 1, 1, 1);

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) col += sampleTex[i] * kernel[i];

    return vec4(col, 1.0);
}

void main() {
    vec4 outColor;
    switch (effect) {
        case 0:
            outColor = texture(screenTexture, TexCoords);
            break;
        case 1:
            outColor = inversion();
            break;
        case 2:
            outColor = grayscale();
            break;
        case 3:
            outColor = kernal1();
            break;
        case 4: outColor = kernal2(); break;
        default:
            outColor = texture(screenTexture, TexCoords);
            break;
    }
    FragColor = outColor;
}

//FRAGMENT_END
