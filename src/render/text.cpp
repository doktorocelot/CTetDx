#include "text.hpp"

static void text_initGlyphsFromBin(Glyph *glyphs, unsigned char *data, const size_t glyphCount) {
    // assumes glyphs arg has enough space for ascii charset
    constexpr int LINE_LEN = 40;
    for (int i = 0; i < glyphCount; i++) {
        const int index = *reinterpret_cast<int *>(data);
        glyphs[index].advance = *reinterpret_cast<float *>(data + sizeof(int));
        glyphs[index].triBounds.left = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float));
        glyphs[index].triBounds.bottom = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 2);
        glyphs[index].triBounds.right = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 3);
        glyphs[index].triBounds.top = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 4);
        glyphs[index].pixelOffset.left = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 5);
        glyphs[index].pixelOffset.bottom = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 6);
        glyphs[index].pixelOffset.right = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 7);
        glyphs[index].pixelOffset.top = *reinterpret_cast<float *>(data + sizeof(int) + sizeof(float) * 8);
        data += LINE_LEN;
    }
}
static constexpr size_t CTF_HEADER_SIZE = 4;
static constexpr size_t CTF_RESOLUTION_OFFSET = CTF_HEADER_SIZE + sizeof(unsigned int);
static constexpr size_t CTF_DISTANCE_RANGE_OFFSET = CTF_HEADER_SIZE + sizeof(unsigned int) * 2;
static constexpr size_t CTF_DATA_OFFSET = CTF_HEADER_SIZE + sizeof(unsigned int) * 3;
void textRenderer_init(
    TextRenderer *textRenderer,
    unsigned char *rawGlyphData
) {
    *textRenderer = {};
    
    size_t glyphCount = *reinterpret_cast<unsigned int *>(rawGlyphData + CTF_HEADER_SIZE);
    textRenderer->resolution = *reinterpret_cast<unsigned int *>(rawGlyphData + CTF_RESOLUTION_OFFSET);
    textRenderer->distanceRange = *reinterpret_cast<unsigned int *>(rawGlyphData + CTF_DISTANCE_RANGE_OFFSET);
    text_initGlyphsFromBin(textRenderer->glyphs, rawGlyphData + CTF_DATA_OFFSET, glyphCount);
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
