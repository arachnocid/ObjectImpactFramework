#pragma once
#include "RuleManager.h"

namespace OIF
{
    class ActivateSink : public RE::BSTEventSink<RE::TESActivateEvent>
    {
    public:
        static ActivateSink* GetSingleton()
        {
            static ActivateSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESActivateEvent* evn,
            RE::BSTEventSource<RE::TESActivateEvent>*) override;
    };

    class HitSink : public RE::BSTEventSink<RE::TESHitEvent>
    {
    public:
        static HitSink* GetSingleton()
        {
            static HitSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESHitEvent* evn,
            RE::BSTEventSource<RE::TESHitEvent>*) override;
    };

    void RegisterSinks();
}
