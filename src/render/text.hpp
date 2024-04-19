#pragma once
#include "../math/rect.hpp"
#include "../math/vector2.hpp"

#define GLYPH_LUT_LENGTH 128

#define CHAR_QUAD_VERT_COUNT 4
#define MAX_CHAR_QUADS 1024

struct Glyph {
    float advance;
    Rect triBounds;
    Rect pixelOffset;
};

struct CharVertex {
    Vector2 position;
    Vector2 texCoords;
};

struct TextRenderer {
    Glyph glyphs[GLYPH_LUT_LENGTH];
    unsigned int resolution;
    unsigned int distanceRange;
    CharVertex chars[MAX_CHAR_QUADS][CHAR_QUAD_VERT_COUNT];
    size_t activeCharCount;
};

enum TextAlignment {
    TextAlignment_LEFT,
    TextAlignment_CENTER,
    TextAlignment_RIGHT,
};

struct Text {
    const char *string;
    Vector2 position;
    float size;
    TextAlignment alignment;
};

void textRenderer_init(
    TextRenderer *textRenderer,
    unsigned char *rawGlyphData
);

void textRenderer_stageText(TextRenderer *textRenderer, const Text *texts,
                            size_t count);

void textRenderer_clearText(TextRenderer *textRenderer);
