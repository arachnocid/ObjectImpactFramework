#pragma once
#include "RuleManager.h"
#include "RE/T/TESDestructionStageChangedEvent.h"

namespace OIF
{
	
//██╗░░██╗███████╗██╗░░░░░██████╗░███████╗██████╗░░██████╗
//██║░░██║██╔════╝██║░░░░░██╔══██╗██╔════╝██╔══██╗██╔════╝
//███████║█████╗░░██║░░░░░██████╔╝█████╗░░██████╔╝╚█████╗░
//██╔══██║██╔══╝░░██║░░░░░██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗
//██║░░██║███████╗███████╗██║░░░░░███████╗██║░░██║██████╔╝
//╚═╝░░╚═╝╚══════╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░

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
			if (!baseObj) return false;
            return IsRelevantObject(baseObj);
        }

		static bool IsActorSafe(RE::Actor* actor) {
			if (!actor || actor->IsDeleted() || actor->IsDead() || !actor->GetBaseObject()) return false;
			try {
				auto* baseObj = actor->GetBaseObject();
				if (!baseObj) return false;
  
				auto* cell = actor->GetParentCell();
				if (!cell) return false;
    
				return true;
			} catch (...) {
				return false;
			}
		}

		static bool IsItemSafe(RE::TESObjectREFR* item)
		{
			if (!item || item->IsDeleted() || !item->GetBaseObject()) return false;
			if (!EventSinkBase::IsRelevantObjectRef(item)) return false;
			try {
				auto* baseObj = item->GetBaseObject();
				if (!baseObj) return false;

				auto* cell = item->GetParentCell();
				if (!cell) return false;

				return true;
			} catch (...) {
				return false;
			}
		}
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


//░██████╗██╗███╗░░██╗██╗░░██╗░██████╗
//██╔════╝██║████╗░██║██║░██╔╝██╔════╝
//╚█████╗░██║██╔██╗██║█████═╝░╚█████╗░
//░╚═══██╗██║██║╚████║██╔═██╗░░╚═══██╗
//██████╔╝██║██║░╚███║██║░╚██╗██████╔╝
//╚═════╝░╚═╝╚═╝░░╚══╝╚═╝░░╚═╝╚═════╝░                              

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
		static constexpr std::uint32_t HK_PROPERTY_DROPPEDOBJECT{ 271828 };
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

	class DestructionStageChangedSink : public RE::BSTEventSink<RE::TESDestructionStageChangedEvent>
	{
	public:
		static DestructionStageChangedSink* GetSingleton()
		{
			static DestructionStageChangedSink sink;
			return &sink;
		}

		RE::BSEventNotifyControl ProcessEvent(
			const RE::TESDestructionStageChangedEvent* evn,
			RE::BSTEventSource<RE::TESDestructionStageChangedEvent>*) override;
	};

	/*class DropSink : public RE::BSTEventSink<RE::TESContainerChangedEvent>
	{
	public:
		static DropSink* GetSingleton()
		{
			static DropSink sink;
			return &sink;
		}
		RE::BSEventNotifyControl ProcessEvent(
			const RE::TESContainerChangedEvent* evn,
			RE::BSTEventSource<RE::TESContainerChangedEvent>*) override;
	};*/

	
//██╗░░██╗░█████╗░░█████╗░██╗░░██╗░██████╗
//██║░░██║██╔══██╗██╔══██╗██║░██╔╝██╔════╝
//███████║██║░░██║██║░░██║█████═╝░╚█████╗░
//██╔══██║██║░░██║██║░░██║██╔═██╗░░╚═══██╗
//██║░░██║╚█████╔╝╚█████╔╝██║░╚██╗██████╔╝
//╚═╝░░╚═╝░╚════╝░░╚════╝░╚═╝░░╚═╝╚═════╝░                               

	struct ExplosionHook
	{
		static void thunk(RE::Explosion* a_this);
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size = 0xA2;
	};

	// Taken and adapted from Rain Extinguishes Fires source code
	struct WeatherChangeHook
	{
		static void thunk(RE::TESRegion* a_region, RE::TESWeather* a_currentWeather);
		static inline REL::Relocation<decltype(thunk)> func;
		static inline RE::TESWeather* currentWeather{ nullptr };
	};  

    // Taken and adapted from Grab and Throw source code
    struct ReadyWeaponHook {
		static void thunk(RE::ReadyWeaponHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t size = 0x4;
    };

    struct UpdateHook
    {
        static void thunk(RE::PlayerCharacter* a_this);
        static inline REL::Relocation<decltype(thunk)> func;
        static constexpr std::size_t size = 0xAD;
    };

	struct AttackBlockHook
	{
		static void thunk(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size = 0x4;
	};

	// Taken and adapted from Explosion Collision Fix source code
	//struct ProcessProjectileHitHook
	//{
	//	static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, RE::NiPoint3* a_hitPos, RE::hkVector4* a_arg, 
	//					  RE::COL_LAYER a_collisionLayer, RE::MATERIAL_ID a_materialID, bool* a_handled);
	//	static inline REL::Relocation<decltype(thunk)> func;
	//};

	// Credits to RavenKZP for the following hooks!
	struct MissileImpact
	{
		static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
						  std::int32_t a_arg6, std::uint32_t a_arg7);		
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct BeamImpact
	{
		static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
						  std::int32_t a_arg6, std::uint32_t a_arg7);		
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct FlameImpact
	{
		static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
						  std::int32_t a_arg6, std::uint32_t a_arg7);		
		static inline REL::Relocation<decltype(thunk)> func;
	};

	//struct GrenadeImpact
	//{
	//	static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
	//					  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
	//					  std::int32_t a_arg6, std::uint32_t a_arg7);
	//	static inline REL::Relocation<decltype(thunk)> func;
	//};

	//struct ConeImpact
	//{
	//	static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
	//					  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
	//					  std::int32_t a_arg6, std::uint32_t a_arg7);
	//	static inline REL::Relocation<decltype(thunk)> func;
	//};

	struct ArrowImpact
	{
		static void thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable, 
						  std::int32_t a_arg6, std::uint32_t a_arg7);		
		static inline REL::Relocation<decltype(thunk)> func;
	};


//██████╗░███████╗░██████╗░██╗░██████╗████████╗██████╗░░█████╗░████████╗██╗░█████╗░███╗░░██╗
//██╔══██╗██╔════╝██╔════╝░██║██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██║██╔══██╗████╗░██║
//██████╔╝█████╗░░██║░░██╗░██║╚█████╗░░░░██║░░░██████╔╝███████║░░░██║░░░██║██║░░██║██╔██╗██║
//██╔══██╗██╔══╝░░██║░░╚██╗██║░╚═══██╗░░░██║░░░██╔══██╗██╔══██║░░░██║░░░██║██║░░██║██║╚████║
//██║░░██║███████╗╚██████╔╝██║██████╔╝░░░██║░░░██║░░██║██║░░██║░░░██║░░░██║╚█████╔╝██║░╚███║
//╚═╝░░╚═╝╚══════╝░╚═════╝░╚═╝╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚══╝   

    void RegisterSinks();
    void InstallHooks();
}
