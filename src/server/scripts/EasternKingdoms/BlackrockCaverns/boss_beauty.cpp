
/*
 * Copyright (C) 2011 - 2013 Naios <https://github.com/Naios>
 *
 * THIS particular file is NOT free software.
 * You are not allowed to share or redistribute it.
 */

#include "ScriptPCH.h"
#include "blackrock_caverns.h"

enum Spells
{
    // Beauty
    SPELL_BERSERK                   = 82395,
    SPELL_MAGMA_SPIT                = 76031,

    // Runty
    SPELL_ALMOST_FEROCIOUS          = 77783,
};

enum Events
{
    // Beauty
    EVENT_BERSERK                   = 1,
    EVENT_BERSEKER_CHARGE           = 2,
    EVENT_FLAMEBREAK                = 3,
    EVENT_MAGMA_SPIT                = 4,
    EVENT_TERRIFYING_ROAR           = 5,

    // Whelps
    EVENT_LAVA_DROOL                = 6,
    EVENT_LITTLE_BREATH             = 7
};

enum Actions
{
    ACTION_BEAUTY_BERSERK   = 1
};

#define SPELL_TERRIFYING_ROAR   RAID_MODE(76028, 93586)
#define SPELL_CHARGE            RAID_MODE(76030, 93580)
#define SPELL_FLAMEBREAK        RAID_MODE(76032, 93583)
#define SPELL_LAVA_DOOL         RAID_MODE(76628, 93666)
#define SPELL_LITTLE_BREATH     RAID_MODE(76665, 93667)

class boss_beauty : public CreatureScript
{
public:
    boss_beauty() : CreatureScript("boss_beauty") { }

    struct boss_beautyAI : public BossAI
    {
        boss_beautyAI(Creature* creature) : BossAI(creature, DATA_BEAUTY) {}

        void Reset()
        {
            _Reset();
            RemoveEncounterFrame();
        }

        void EnterCombat(Unit* who)
        {
            _EnterCombat();
            AddEncounterFrame();

            events.ScheduleEvent(EVENT_BERSEKER_CHARGE, urand(15000, 24000), 0, 0);
            events.ScheduleEvent(EVENT_FLAMEBREAK, urand(10000, 21000), 0, 0);
            events.ScheduleEvent(EVENT_MAGMA_SPIT, urand(9000, 13000), 0, 0);
            events.ScheduleEvent(EVENT_TERRIFYING_ROAR, 30000, 0, 0);

            if (me->GetMap()->IsHeroic())
            {
                // Find all adds
                std::list<Unit*> targets;
                Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(me, me, 80.0f);
                Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
                me->VisitNearbyObject(80.0f, searcher);
                for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                {
                    if ((*itr))
                        if ((*itr)->ToCreature() && (*itr)->ToCreature()->GetScriptName() == "npc_beauty_whelp")
                            (*itr)->ToCreature()->SetInCombatWithZone();
                }
            }
        }

        void DoAction(int32 action)
        {
            if (action == ACTION_BEAUTY_BERSERK && !me->HasAura(SPELL_BERSERK))
            {
                DoCast(me, SPELL_BERSERK);
                events.ScheduleEvent(EVENT_BERSERK, 1000, 0, 0);
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BERSEKER_CHARGE:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            me->CastSpell(target, SPELL_CHARGE, true);
                            events.ScheduleEvent(EVENT_BERSEKER_CHARGE, urand(12000, 20000), 0, 0);
                        }
                        break;
                    }
                    case EVENT_FLAMEBREAK:
                    {
                        DoCastAOE(SPELL_FLAMEBREAK);
                        events.ScheduleEvent(EVENT_FLAMEBREAK, urand(5000, 9000), 0, 0);
                        break;
                    }
                    case EVENT_MAGMA_SPIT:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            me->CastSpell(target, SPELL_MAGMA_SPIT);
                            events.ScheduleEvent(EVENT_MAGMA_SPIT, urand(9000, 13000), 0, 0);
                        }
                        break;
                    }
                    case EVENT_TERRIFYING_ROAR:
                    {
                        DoCast(me, SPELL_TERRIFYING_ROAR, true);
                        events.ScheduleEvent(EVENT_TERRIFYING_ROAR, 30000, 0, 0);
                        break;
                    }
                    case EVENT_BERSERK:
                    {
                        DoCast(me, SPELL_BERSERK);
                        break;
                    }
                }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            RemoveEncounterFrame();

            if (instance)
                instance->SetBossState(DATA_BEAUTY, DONE);

            _JustDied();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_beautyAI (creature);
    }
};

class npc_runty : public CreatureScript
{
public:
    npc_runty() : CreatureScript("npc_runty") { }

    struct npc_runtyAI : public ScriptedAI
    {
        npc_runtyAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) {}

        InstanceScript* instance;

        void Reset()
        {
            me->RemoveAurasDueToSpell(SPELL_ALMOST_FEROCIOUS);
        }

        void EnterCombat(Unit* who)
        {
            DoCast(me, SPELL_ALMOST_FEROCIOUS);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance && me->GetMap()->IsHeroic())
            {
                if (Creature* beauty = ObjectAccessor::GetCreature(*me, instance->GetData64(BOSS_BEAUTY)))
                    beauty->AI()->DoAction(ACTION_BEAUTY_BERSERK);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runtyAI (creature);
    }
};

class npc_beauty_whelp : public CreatureScript
{
public:
    npc_beauty_whelp() : CreatureScript("npc_beauty_whelp") { }

    struct npc_beauty_whelpAI : public ScriptedAI
    {
        npc_beauty_whelpAI(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()) {}

        InstanceScript* instance;
        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* who)
        {
            if (instance && me->GetMap()->IsHeroic())
            {
                if (Creature* beauty = ObjectAccessor::GetCreature(*me, instance->GetData64(BOSS_BEAUTY)))
                {
                    beauty->SetInCombatWithZone();
                    beauty->GetMotionMaster()->MoveChase(who, 2.0f, 0.0f);
                }
            }
            events.ScheduleEvent(EVENT_LAVA_DROOL, urand(10000, 20000), 0, 0);
            events.ScheduleEvent(EVENT_LITTLE_BREATH, urand(6000, 12000), 0, 0);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_LAVA_DROOL:
                    {
                        DoCast(me, SPELL_LAVA_DOOL);
                        events.ScheduleEvent(EVENT_LAVA_DROOL, urand(10000, 20000), 0, 0);
                        break;
                    }
                    case EVENT_LITTLE_BREATH:
                    {
                        DoCastVictim(SPELL_LITTLE_BREATH);
                        events.ScheduleEvent(EVENT_LITTLE_BREATH, urand(6000, 12000), 0, 0);
                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_beauty_whelpAI (creature);
    }
};

class spell_brc_magma_spit : public SpellScriptLoader
{
public:
    spell_brc_magma_spit() : SpellScriptLoader("spell_brc_magma_spit")
    {
    }

    enum spellId
    {
        SPELL_MAGMA_BLAST_BR    = 76058,
        SPELL_MAGMA_BLAST_FR    = 76072,
        SPELL_MAGMA_BLAST_L     = 76074,
        SPELL_MAGMA_BLAST_BL    = 76076
    };

    class spell_brc_magma_spit_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_brc_magma_spit_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
            {
                target->CastSpell(target, SPELL_MAGMA_BLAST_BR, true);
                target->CastSpell(target, SPELL_MAGMA_BLAST_FR, true);
                target->CastSpell(target, SPELL_MAGMA_BLAST_L, true);
                target->CastSpell(target, SPELL_MAGMA_BLAST_BL, true);
            }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_brc_magma_spit_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_brc_magma_spit_AuraScript();
    }
};

void AddSC_boss_beauty()
{
    new boss_beauty();
    new npc_runty();
    new npc_beauty_whelp();
    new spell_brc_magma_spit();
}
