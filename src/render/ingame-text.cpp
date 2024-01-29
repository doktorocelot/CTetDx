#include "ingame-text.hpp"

#include "ctet/ctet.h"

#include <cstdio>
#include <cstring>

#define INDEX_NEXT 0
#define INDEX_HOLD 1
#define INDEX_SCORE_LABEL 2

const static char *NEXT_TEXT = "NEXT";
const static char *HOLD_TEXT = "HOLD";

const static char *SCORE_TEXT = "Score";
const static char *LEVEL_TEXT = "Speed Lv.";
const static char *LINES_TEXT = "Lines";
const static char *PIECES_TEXT = "Pieces";

void ingameText_init(IngameText *ingameText) {
    const auto texts = ingameText->texts;
    constexpr float baseX = CT_FIELD_WIDTH / 2 + 3;
    constexpr float baseY = CT_VISIBLE_FIELD_HEIGHT / 2 + 0.5;
    texts[INDEX_NEXT] = {
        .string = NEXT_TEXT,
        .position = {baseX, baseY},
        .size = 1.2,
        .alignment = TextAlignment_CENTER,
    };
    texts[INDEX_HOLD] = {
        .string = HOLD_TEXT,
        .position = {-baseX, baseY},
        .size = 1.2,
        .alignment = TextAlignment_CENTER,
    };

    constexpr int totalStats = 4;
    const char *labels[] = {SCORE_TEXT, LEVEL_TEXT, LINES_TEXT, PIECES_TEXT};
    const char *values[] = {
        ingameText->scoreValueText,
        ingameText->levelValueText,
        ingameText->linesValueText,
        ingameText->piecesValueText,
    };
    const float sizes[] = {1, 1.5f, 1.5f, 1.5f};
    int index = INDEX_SCORE_LABEL;
    for (int i = 0; i < totalStats; i++) {
        constexpr float baseX = -(CT_FIELD_WIDTH / 2 + 1);
        const float baseY = -3.0f * i;
        texts[index] = {
            .string = labels[i],
            .position = {baseX, baseY},
            .size = 1,
            .alignment = TextAlignment_RIGHT,
        };

        texts[index + 1] = {
            .string = values[i],
            .position = {baseX, baseY - sizes[i]},
            .size = sizes[i],
            .alignment = TextAlignment_RIGHT,
        };

        index += 2;
    }

}

void ingameText_update(IngameText *ingameText, const CTetStats *stats) {
    // char scoreTextUnformatted[SCORE_VALUE_TEXT_BYTES];
    // snprintf(scoreTextUnformatted, SCORE_VALUE_TEXT_BYTES, "%d", stats->score);
    // Comma Separators for Score TODO
    // const size_t n = strlen(scoreTextUnformatted);
    // int commaAccum = n < 4 ? 0 : n % 8 == 0 || n % 8 == 4 ? 2 : 1;
    // int originalTextIndex = 0;
    // for (int i = 0; i < SCORE_VALUE_TEXT_BYTES; i++) {
    //     if (scoreTextUnformatted[originalTextIndex] == '\0') {
    //         ingameText->scoreValueText[i] = '\0';
    //         break;
    //     }
    //     if (commaAccum == 3) {
    //         ingameText->scoreValueText[i] = ',';
    //         commaAccum = 0;
    //         continue;
    //     }
    //     ingameText->scoreValueText[i] = scoreTextUnformatted[originalTextIndex];
    //     originalTextIndex++;
    //     commaAccum++;
    // }
    
    snprintf(ingameText->scoreValueText, SCORE_VALUE_TEXT_BYTES, "%lld", stats->score);
    snprintf(ingameText->levelValueText, REGULAR_VALUE_TEXT_BYTES, "%d", stats->level);
    snprintf(ingameText->linesValueText, REGULAR_VALUE_TEXT_BYTES, "%d", stats->lines);
    snprintf(ingameText->piecesValueText, REGULAR_VALUE_TEXT_BYTES, "%d", stats->pieces);
}
