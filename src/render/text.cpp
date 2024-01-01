#include "text.hpp"

static void text_initGlyphsFromBin(Glyph *glyphs, unsigned char *data, const size_t dataLength, const float resolution) {
    // assumes glyphs arg has enough space for ascii charset
    constexpr int LINE_LEN = 40;
    const int totalLines = dataLength / LINE_LEN;
    for (int i = 0; i < totalLines; i++) {
        const int index = *reinterpret_cast<int *>(data);
        glyphs[index].advance = *reinterpret_cast<float *>(data + sizeof(int));
        glyphs[index].triBounds.left = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float));
        glyphs[index].triBounds.bottom = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 2);
        glyphs[index].triBounds.right = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 3);
        glyphs[index].triBounds.top = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 4);
        glyphs[index].pixelOffset.left = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 5) / resolution;
        glyphs[index].pixelOffset.bottom = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 6) / resolution;
        glyphs[index].pixelOffset.right = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 7) / resolution;
        glyphs[index].pixelOffset.top = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 8) / resolution;
        data += LINE_LEN;
    }
}

void textRenderer_init(
    TextRenderer *textRenderer,
    unsigned char *rawGlyphData,
    const size_t dataLen,
    const float imgResolution
    ) {
    *textRenderer = {};
    text_initGlyphsFromBin(textRenderer->glyphs, rawGlyphData, dataLen, imgResolution);
}

void textRenderer_setText(TextRenderer *textRenderer, Text *texts, size_t textCount) {
    Vector2 caret = {};
    int charIndex = 0;
    textRenderer->activeCharCount = 0;
    for (int i = 0; i < textCount; i++) {
        const Text text = texts[i];
        const char *string = text.string;
        caret = text.position;
        const int size = text.size;
        const TextAlignment alignment = text.alignment;
        const int startingCharIndex = charIndex;
        int startingX = caret.x;
        
        while (*string != '\0') {
            if (*string < ' ') {
                string++;
                continue;
            }
            if (*string == ' ') {
                caret.x += textRenderer->glyphs[' '].advance;
                string++;
                continue;
            }
            const auto [advance, triBounds, pixelOffset] = textRenderer->glyphs[*string];
        
            textRenderer->chars[charIndex][0].position = vector2_addToNew({triBounds.left * size, triBounds.bottom * size}, caret);
            textRenderer->chars[charIndex][1].position = vector2_addToNew({triBounds.right * size, triBounds.bottom * size}, caret);
            textRenderer->chars[charIndex][2].position = vector2_addToNew({triBounds.left * size, triBounds.top * size}, caret);
            textRenderer->chars[charIndex][3].position = vector2_addToNew({triBounds.right * size, triBounds.top * size}, caret);

            textRenderer->chars[charIndex][0].texCoords = {pixelOffset.left, pixelOffset.bottom};
            textRenderer->chars[charIndex][1].texCoords = {pixelOffset.right, pixelOffset.bottom};
            textRenderer->chars[charIndex][2].texCoords = {pixelOffset.left, pixelOffset.top};
            textRenderer->chars[charIndex][3].texCoords = {pixelOffset.right, pixelOffset.top};

            caret.x += advance * size;
        
            textRenderer->activeCharCount++;
            charIndex++;
            string++;
        }
        
        if (alignment != TextAlignment_LEFT) {
            float shift = caret.x - startingX;
            if (alignment == TextAlignment_CENTER) shift /= 2;
            const int total = charIndex;
            for (int j = startingCharIndex; j < total; j++) {
                textRenderer->chars[j][0].position.x -= shift;
                textRenderer->chars[j][1].position.x -= shift;
                textRenderer->chars[j][2].position.x -= shift;
                textRenderer->chars[j][3].position.x -= shift;
            }
        }
    }

}
