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

void textRenderer_setText(TextRenderer *textRenderer, const char *text) {
    Vector2 caret = {};
    int i = 0;
    textRenderer->activeCharCount = 0;
    while (*text != '\0') {
        if (*text < ' ') {
            text++;
            continue;
        }
        if (*text == ' ') {
            caret.x += textRenderer->glyphs[' '].advance;
            text++;
            continue;
        }
        const Glyph glyph = textRenderer->glyphs[*text];
        
        textRenderer->chars[i][0].position = vector2_addToNew({glyph.triBounds.left, glyph.triBounds.bottom}, caret);
        textRenderer->chars[i][1].position = vector2_addToNew({glyph.triBounds.right, glyph.triBounds.bottom}, caret);
        textRenderer->chars[i][2].position = vector2_addToNew({glyph.triBounds.left, glyph.triBounds.top}, caret);
        textRenderer->chars[i][3].position = vector2_addToNew({glyph.triBounds.right, glyph.triBounds.top}, caret);

        textRenderer->chars[i][0].texCoords = {glyph.pixelOffset.left, glyph.pixelOffset.bottom};
        textRenderer->chars[i][1].texCoords = {glyph.pixelOffset.right, glyph.pixelOffset.bottom};
        textRenderer->chars[i][2].texCoords = {glyph.pixelOffset.left, glyph.pixelOffset.top};
        textRenderer->chars[i][3].texCoords = {glyph.pixelOffset.right, glyph.pixelOffset.top};

        caret.x += glyph.advance;
        
        textRenderer->activeCharCount++;
        i++;
        text++;
    }
}
