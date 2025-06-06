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

    class InputHandler : public RE::BSTEventSink<RE::InputEvent*>
    {
    public:
        static InputHandler* GetSingleton()
        {
            static InputHandler handler;
            return &handler;
        }

        virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
        
        void Register();
        void Unregister();
        
        bool WasKeyJustReleased() const { return KeyJustReleased; }
        void ResetKeyState() { 
            KeyJustReleased = false; 
        }

    private:
        bool KeyWasPressed = false;
        bool KeyJustReleased = false;
        
        static constexpr std::uint32_t R_KEY_CODE = 19;
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
}
