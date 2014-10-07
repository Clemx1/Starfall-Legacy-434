﻿
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "SpellScript.h"
#include "MoveSplineInit.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "halls_of_origination.h"

class go_hoo_lift_console : public GameObjectScript
{
public:
    go_hoo_lift_console() : GameObjectScript("go_hoo_lift_console") { }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
    {
            if (sender != GOSSIP_SENDER_MAIN)
                return false;
            if (!player->getAttackers().empty())
                return false;
        switch (action)
        {
            case 200: // Bring to first floor
                if (player->GetPositionZ() > 90.0f)
                    if (GameObject* elevator = go->FindNearestGameObject(GO_LIFT_OF_THE_MAKERS, 500.0f))
                    {
                        elevator->SetLootState(GO_READY);
                        elevator->UseDoorOrButton(5000);
                    }
                break;
            case 201: // Bring to second floor
                if (player->GetPositionZ() < 90.0f)
                    if (GameObject* elevator = go->FindNearestGameObject(GO_LIFT_OF_THE_MAKERS, 500.0f))
                    {
                        elevator->SetLootState(GO_READY);
                        elevator->UseDoorOrButton(5000);
                    }
                break;
        }
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {

        if (InstanceScript* instance = go->GetInstanceScript())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bring elevator to first floor.", GOSSIP_SENDER_MAIN, 200);
            if (instance->GetBossState(DATA_ANRAPHET) == DONE || instance->GetBossState(DATA_EARTHRAGER_PTAH) == DONE) // if anraphet or ptah is defeated
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bring elevator to second floor.", GOSSIP_SENDER_MAIN, 201);
        }

        player->SEND_GOSSIP_MENU(go->GetGOInfo()->GetGossipMenuId(), go->GetGUID());
        return true;
    }
};

class spell_hoo_transit_device : public SpellScriptLoader
{
    public:
        spell_hoo_transit_device() : SpellScriptLoader("spell_hoo_transit_device") { }

        class spell_hoo_transit_device_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hoo_transit_device_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* caster = GetCaster())
                    if (InstanceScript* instance = caster->GetInstanceScript())
                        if (!caster->isInCombat())
                            if (instance->GetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR) == DONE)
                                return SPELL_CAST_OK;

                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_hoo_transit_device_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hoo_transit_device_SpellScript();
        }
};

void AddSC_halls_of_origination()
{
    new go_hoo_lift_console();
    new spell_hoo_transit_device();
}
