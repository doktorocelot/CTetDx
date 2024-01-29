#pragma once
#include "wav.hpp"

enum Sounds {
    Sounds_HARD_DROP,
    Sounds_SHIFT,
    Sounds_LOCK,
    Sounds_ROTATE,
    Sounds_HOLD,
    Sounds_ERASE1,
    Sounds_ERASE2,
    Sounds_ERASE3,
    Sounds_ERASE4,
    Sounds_LENGTH,
};

void loadSounds(PcmS16Buffer soundBuffers[Sounds_LENGTH]);