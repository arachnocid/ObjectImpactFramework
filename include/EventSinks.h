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

    class GrabReleaseSink : public RE::BSTEventSink<RE::TESGrabReleaseEvent>
    {
    public:
        static GrabReleaseSink* GetSingleton()
        {
            static GrabReleaseSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESGrabReleaseEvent* evn,
            RE::BSTEventSource<RE::TESGrabReleaseEvent>*) override;
    };

    class TelekinesisLandingSink : public RE::hkpContactListener
    {
    public:
        static TelekinesisLandingSink* GetSingleton()
        {
            static TelekinesisLandingSink sink;
            return &sink;
        }

        void ContactPointCallback(const RE::hkpContactPointEvent& evn) override;

    private:
        std::unordered_set<std::uint32_t> processedObjects;
        std::vector<RE::hkpRigidBody*> bodiesToCleanup;
    };

    void RegisterSinks();
}
