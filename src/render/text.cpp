#include "text.hpp"

static float loadFloatFromData(unsigned char **data) {
    const float value = *reinterpret_cast<float *>(*data);
    *data += sizeof(float);
    return value;
}

static float loadUIntFromData(unsigned char **data) {
    const unsigned int value = *reinterpret_cast<unsigned int *>(*data);
    *data += sizeof(unsigned int);
    return value;
}

static void text_initGlyphsFromBin(Glyph *glyphs, unsigned char *data, const size_t glyphCount) {
    // assumes glyphs arg has enough space for ascii charset
    for (int i = 0; i < glyphCount; i++) {
        const int index = loadUIntFromData(&data);
        glyphs[index].advance = loadFloatFromData(&data);
        glyphs[index].triBounds.left = loadFloatFromData(&data);
        glyphs[index].triBounds.bottom = loadFloatFromData(&data);
        glyphs[index].triBounds.right = loadFloatFromData(&data);
        glyphs[index].triBounds.top = loadFloatFromData(&data);
        glyphs[index].pixelOffset.left = loadFloatFromData(&data);
        glyphs[index].pixelOffset.bottom = loadFloatFromData(&data);
        glyphs[index].pixelOffset.right = loadFloatFromData(&data);
        glyphs[index].pixelOffset.top = loadFloatFromData(&data);
    }
}

static constexpr size_t CTF_HEADER_SIZE = 4;

void textRenderer_init(
    TextRenderer *textRenderer,
    unsigned char *rawGlyphData
) {
    *textRenderer = {};
    rawGlyphData += CTF_HEADER_SIZE;
    size_t glyphCount = loadUIntFromData(&rawGlyphData);
    textRenderer->resolution = loadUIntFromData(&rawGlyphData);
    textRenderer->distanceRange = loadUIntFromData(&rawGlyphData);
    text_initGlyphsFromBin(textRenderer->glyphs, rawGlyphData, glyphCount);
}

void textRenderer_clearText(TextRenderer *textRenderer) {
    textRenderer->activeCharCount = 0;
}

void textRenderer_stageText(TextRenderer *textRenderer, const Text *texts, const size_t count) {
    int charIndex = textRenderer->activeCharCount;
    for (int i = 0; i < count; i++) {
        if (charIndex >= MAX_CHAR_QUADS) return;
        
        const Text text = texts[i];
        
        const char *string = text.string;
        if (string == nullptr) continue;
        
        Vector2 caret = text.position;
        const float size = text.size;

        // string length for alignment
        float lineLen = 0; 
        int stringLen = 0;
        while (*string != '\0') {
            if (const char current = *string; current >= ' ') {
                lineLen += textRenderer->glyphs[current].advance * size;                
            }
            stringLen++;
            string++;
        }
        string -= stringLen;

        // alignment
        switch (text.alignment) {
        case TextAlignment_CENTER:
            caret.x -= lineLen / 2;
            break;
        case TextAlignment_RIGHT:
            caret.x -= lineLen;
            break;
        default:
            break;
        }

        // create quads
        for (int c = 0; c < stringLen; c++) {
            const char currentChar = string[c];
            if (currentChar < ' ') {
                continue;
            }
            if (currentChar == ' ') {
                caret.x += textRenderer->glyphs[' '].advance * size;
                continue;
            }
            const auto [advance, triBounds, pixelOffset] = textRenderer->glyphs[currentChar];
        
            textRenderer->chars[charIndex][0].position = vector2_addToNew({triBounds.left * size, triBounds.bottom * size}, caret);
            textRenderer->chars[charIndex][1].position = vector2_addToNew({triBounds.right * size, triBounds.bottom * size}, caret);
            textRenderer->chars[charIndex][2].position = vector2_addToNew({triBounds.left * size, triBounds.top * size}, caret);
            textRenderer->chars[charIndex][3].position = vector2_addToNew({triBounds.right * size, triBounds.top * size}, caret);

            textRenderer->chars[charIndex][0].texCoords = {pixelOffset.left, pixelOffset.bottom};
            textRenderer->chars[charIndex][1].texCoords = {pixelOffset.right, pixelOffset.bottom};
            textRenderer->chars[charIndex][2].texCoords = {pixelOffset.left, pixelOffset.top};
            textRenderer->chars[charIndex][3].texCoords = {pixelOffset.right, pixelOffset.top};

            caret.x += advance * size;
            
            charIndex++;
        }
        
    }
    textRenderer->activeCharCount = charIndex;
}
