#pragma once

#include "RE/N/NiSmartPointer.h"

namespace RE {
    class TESObjectREFR;

    struct TESDestructionStageChangedEvent {
    public:

        TESDestructionStageChangedEvent();
        TESDestructionStageChangedEvent(TESObjectREFR* target, uint32_t oldStage, uint32_t newStage);
        ~TESDestructionStageChangedEvent() = default;

        RE::NiPointer<RE::TESObjectREFR> target;
        uint32_t oldStage;
        uint32_t newStage;
    };
}