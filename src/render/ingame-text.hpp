#pragma once

#include "text.hpp"
#include "ctet/ctet.h"

#define INGAME_TEXT_LEN 16

#define SCORE_VALUE_TEXT_BYTES 30
#define REGULAR_VALUE_TEXT_BYTES 6

struct IngameText {
    Text texts[INGAME_TEXT_LEN];
    char scoreValueText[SCORE_VALUE_TEXT_BYTES];
    char levelValueText[REGULAR_VALUE_TEXT_BYTES];
    char linesValueText[REGULAR_VALUE_TEXT_BYTES];
    char piecesValueText[REGULAR_VALUE_TEXT_BYTES];
};

void ingameText_init(IngameText *ingameText);

void ingameText_update(IngameText *ingameText, const CTetStats *stats);