#pragma once
#include "RuleManager.h"

namespace OIF
{
    class EventSinkBase
    {
    public:
        static const std::unordered_set<RE::FormType> relevantFormTypes;
        
        static bool IsRelevantObject(RE::TESForm* form) {
            if (!form) return false;
            return relevantFormTypes.contains(form->GetFormType());
        }
        
        static bool IsRelevantObjectRef(RE::TESObjectREFR* ref) {
            if (!ref || ref->IsDeleted()) return false;
            auto* baseObj = ref->GetBaseObject();
            return IsRelevantObject(baseObj);
        }
    };

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

    class MagicEffectApplySink : public RE::BSTEventSink<RE::TESMagicEffectApplyEvent>
    {
    public:
        static MagicEffectApplySink* GetSingleton()
        {
            static MagicEffectApplySink sink;
            return &sink;
        }
        
        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESMagicEffectApplyEvent* evn,
            RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*) override;
    };

    struct ExplosionHook {
        static void thunk(RE::Explosion* a_this);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t size = 0xA2;
    };

    struct WeatherChangeHook {
        static void thunk(RE::TESWeather* a_currentWeather);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline RE::TESWeather* currentWeather{ nullptr };
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

    class LandingSink : public RE::hkpContactListener
    {
    public:
        static LandingSink* GetSingleton()
        {
            static LandingSink sink;
            return &sink;
        }

        void ContactPointCallback(const RE::hkpContactPointEvent& evn) override;

    private:
        std::unordered_set<std::uint32_t> processedObjects;
        std::vector<RE::hkpRigidBody*> bodiesToCleanup;
        static constexpr std::uint32_t HK_PROPERTY_TELEKINESIS{ 314159 };
        static constexpr std::uint32_t HK_PROPERTY_GRABTHROWNOBJECT{ 628318 };
    };

    class InputHandler
    {
    public:
        static InputHandler* GetSingleton()
        {
            static InputHandler handler;
            return &handler;
        }
        
        bool WasKeyJustReleased() const { return KeyJustReleased; }
        void SetKeyJustReleased() { KeyJustReleased = true; }
        void ResetKeyState() { KeyJustReleased = false; }
    
    private:
        bool KeyJustReleased = false;
    };

    // Taken and adapted from Grab and Throw source code
    struct ReadyWeaponHook {
        static void thunk(RE::ReadyWeaponHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t size = 0x4;
    };

    class CellAttachDetachSink : public RE::BSTEventSink<RE::TESCellAttachDetachEvent>
    {
    public:
        static CellAttachDetachSink* GetSingleton()
        {
            static CellAttachDetachSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESCellAttachDetachEvent* evn,
            RE::BSTEventSource<RE::TESCellAttachDetachEvent>*) override;
    };

    void RegisterSinks();
    void InstallHooks();
}