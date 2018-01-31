/*
Fixed by Ac-Web Community
Modified by Marcus
Updated by Quadral
Updated 2017 by beimax76
original script created by ??
*/

#include "ScriptPCH.h"
#include "Pet.h"
#include "Bag.h"
#include "Common.h"
#include "Config.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "DBCStructure.h"
#include "Define.h"
#include "Field.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Language.h"
#include "Log.h"
#include "Player.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "QueryResult.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Transaction.h"
#include "WorldSession.h"

#include "Cell.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Unit.h"
#include "GameObject.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Chat.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "SpellScript.h"
#include "SpellInfo.h"
#include "SpellAuraEffects.h"
#include "Language.h"

#include <sstream>
#include <string>

#define GOSSIP_ITEM_STABLE "Stable"
#define GOSSIP_ITEM_NEWPET "New Pet"
#define GOSSIP_ITEM_BOAR "Boar"
#define GOSSIP_ITEM_SERPENT "Serpent"
#define GOSSIP_ITEM_SCRAB "Scrab"
#define GOSSIP_ITEM_LION "Lion"
#define GOSSIP_ITEM_WOLF "Wolf"
#define GOSSIP_ITEM_RAVAGER "Ravenger"
 
#define GOSSIP_ITEM_UNTRAINEPET "Restart Pet"

class Npc_Beastmaster : public CreatureScript
{
public:
        Npc_Beastmaster() : CreatureScript("Npc_Beastmaster") { }

	class BeastmasterAI : public ScriptedAI
	{
	public:
	BeastmasterAI(Creature* _creature) : ScriptedAI(_creature) {}

	void CreatePet(Player *player, Creature * _creature, uint32 entry) {

        if(player->getClass() != CLASS_HUNTER) {
			_creature->TextEmote("You are not a Hunter!", player);
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }
 
        if(player->GetPet()) {
			_creature->TextEmote("First you must drop your pet!", player);
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }
 
		Creature *creatureTarget = _creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY() + 2, player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 500);
		if (!creatureTarget) return;
        
		Pet* pet = player->CreateTamedPetFrom(creatureTarget, 0);
        if(!pet)
			return;
 
        // kill original creature
        creatureTarget->setDeathState(JUST_DIED);
        creatureTarget->RemoveCorpse();
        creatureTarget->SetHealth(0);                       // just for nice GM-mode view
 
		pet->SetPower(POWER_HAPPINESS, 1048000);
 
        //pet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE,0);
        //pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, uint32((Trinity::XP::xp_to_level(70))/4));
 
        // prepare visual effect for levelup
		pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel() - 1);
		pet->GetMap()->AddToMap(pet->ToCreature());
		
        // visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel());
 
        
        if(!pet->InitStatsForLevel(player->getLevel()))
            // sLog->outError("Pet Create fail: no init stats for entry %u", entry);
 
        pet->UpdateAllStats();
        
        // caster have pet now
        player->SetMinion(pet, true);
 
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        pet->InitTalentForLevel();
        player->PetSpellInitialize();
        
        //end
        player->PlayerTalkClass->SendCloseGossip();
		_creature->TextEmote("Pet added!", player);
    }
	
	void CreatureWhisperBasedOnBool(const char *text, Creature *_creature, Player *player, bool value)
		{
			if (value)
			_creature->TextEmote(text, player);
		}
 
	bool GossipHello(Player* player) override
		{
			return OnGossipHello(player, me);
		}
 
    bool OnGossipHello(Player *player, Creature *_creature)
    {
 
        if(player->getClass() != CLASS_HUNTER)
        {
			_creature->TextEmote("You are not a Hunter!", player);
            return true;
        }
        if (player->CanTameExoticPets())
        {
			AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Get a New Exotic Pet.", GOSSIP_SENDER_MAIN, 50);
        }
		
		AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Get a New Pet.", GOSSIP_SENDER_MAIN, 30);
        AddGossipItemFor(player, 2, "Take me to the Stable.", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_STABLEPET);
       /* AddGossipItemFor(player, 6, "Sell me some Food for my Pet.", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_VENDOR);*/
        AddGossipItemFor(player, 5, "Close Beastmaster Window.", GOSSIP_SENDER_MAIN, 150);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
        return true;
    }
	
	bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
		{
			uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
			uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
			return OnGossipSelect(player, me, sender, action);
		}

    bool OnGossipSelect(Player *player, Creature *_creature, uint32 uiSender, uint32 uiAction)
    {
       
	   ClearGossipMenuFor(player);

	if (uiSender == GOSSIP_SENDER_MAIN)
	 {			   
        switch (uiAction)
        {
        
        case 100:
		
		OnGossipHello(player, _creature);

        break;

         case 150:
		 
         CloseGossipMenuFor(player);
						
         break;
	     case 30:
		
            AddGossipItemFor(player, 2, "<- Back to Main Menu.", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 4, "Next Page. ->", GOSSIP_SENDER_MAIN, 31);
            AddGossipItemFor(player, 6, "Bat.", GOSSIP_SENDER_MAIN, 18);
            AddGossipItemFor(player, 6, "Bear.", GOSSIP_SENDER_MAIN, 1);
            AddGossipItemFor(player, 6, "Boar.", GOSSIP_SENDER_MAIN, 2);
            AddGossipItemFor(player, 6, "Cat.", GOSSIP_SENDER_MAIN, 4);
            AddGossipItemFor(player, 6, "Carrion Bird.", GOSSIP_SENDER_MAIN, 5);
            AddGossipItemFor(player, 6, "Crab.", GOSSIP_SENDER_MAIN, 6);
            AddGossipItemFor(player, 6, "Crocolisk.", GOSSIP_SENDER_MAIN, 7);
            AddGossipItemFor(player, 6, "Dragonhawk.", GOSSIP_SENDER_MAIN, 17);
            AddGossipItemFor(player, 6, "Gorilla.", GOSSIP_SENDER_MAIN, 8);
            AddGossipItemFor(player, 6, "Hound.", GOSSIP_SENDER_MAIN, 9);
            AddGossipItemFor(player, 6, "Hyena.", GOSSIP_SENDER_MAIN, 10);
            AddGossipItemFor(player, 6, "Moth.", GOSSIP_SENDER_MAIN, 11);
            AddGossipItemFor(player, 6, "Owl.", GOSSIP_SENDER_MAIN, 12);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
                break;
        
         case 31:
			
            AddGossipItemFor(player, 2, "<- Back to Main Menu.", GOSSIP_SENDER_MAIN, 30);
            AddGossipItemFor(player, 4, "<- Previous Page.", GOSSIP_SENDER_MAIN, 30);
            AddGossipItemFor(player, 6, "Raptor.", GOSSIP_SENDER_MAIN, 20);
            AddGossipItemFor(player, 6, "Ravager.", GOSSIP_SENDER_MAIN, 19);
            AddGossipItemFor(player, 6, "Strider.", GOSSIP_SENDER_MAIN, 13);
            AddGossipItemFor(player, 6, "Scorpid.", GOSSIP_SENDER_MAIN, 414);
            AddGossipItemFor(player, 6, "Spider.", GOSSIP_SENDER_MAIN, 16);
            AddGossipItemFor(player, 6, "Serpent.", GOSSIP_SENDER_MAIN, 21);  
            AddGossipItemFor(player, 6, "Turtle.", GOSSIP_SENDER_MAIN, 15);
            AddGossipItemFor(player, 6, "Wasp.", GOSSIP_SENDER_MAIN, 93);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
                break;
        
         case 50:  
			
            AddGossipItemFor(player, 2, "<- Back to Main Menu.", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 2, "<- Get a New Normal Pet.", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, 6, "Chimaera.", GOSSIP_SENDER_MAIN, 51);
            AddGossipItemFor(player, 6, "Core Hound.", GOSSIP_SENDER_MAIN, 52);
            AddGossipItemFor(player, 6, "Devilsaur.", GOSSIP_SENDER_MAIN, 53);
            AddGossipItemFor(player, 6, "Rhino.", GOSSIP_SENDER_MAIN, 54);
            AddGossipItemFor(player, 6, "Silithid.", GOSSIP_SENDER_MAIN, 55);
            AddGossipItemFor(player, 6, "Worm.", GOSSIP_SENDER_MAIN, 56);  
            AddGossipItemFor(player, 6, "Loque'nahak.", GOSSIP_SENDER_MAIN, 57);
            AddGossipItemFor(player, 6, "Skoll.", GOSSIP_SENDER_MAIN, 58);
            AddGossipItemFor(player, 6, "Gondria.", GOSSIP_SENDER_MAIN, 59);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
         break;
      
            case GOSSIP_OPTION_STABLEPET:
                player->GetSession()->SendStablePet(_creature->GetGUID());
            break; 
            case GOSSIP_OPTION_VENDOR:
               player->GetSession()->SendListInventory(_creature->GetGUID());
            break;
            case 51: //chimera
                CreatePet(player, _creature, 21879);
            break;
            case 52: //core hound
                CreatePet(player, _creature, 21108);
            break;
            case 53: //Devilsaur
                CreatePet(player, _creature, 20931);
            break;
            case 54: //rhino
                CreatePet(player, _creature, 30445);
            break;
            case 55: //silithid
                CreatePet(player, _creature, 5460);
            break;
            case 56: //Worm
                CreatePet(player, _creature, 30148);
            break;
            case 57: //Loque'nahak
                CreatePet(player, _creature, 32517);
            break;
            case 58: //Skoll
                CreatePet(player, _creature, 35189);
            break;
            case 59: //Gondria
                CreatePet(player, _creature, 33776);
            break;
            case 16: //Spider
                CreatePet(player, _creature, 2349);
            break;
            case 17: //Dragonhawk
                CreatePet(player, _creature, 27946);
            break;
            case 18: //Bat
                CreatePet(player, _creature, 28233);
            break;
            case 19: //Ravager
                CreatePet(player, _creature, 17199);
            break;
            case 20: //Raptor
                CreatePet(player, _creature, 14821);
            break;
            case 21: //Serpent
                CreatePet(player, _creature, 28358);
            break;
            case 1: //bear
                CreatePet(player, _creature, 29319);
            break;
            case 2: //Boar
                CreatePet(player, _creature, 29996);
            break;
            case 93: //Bug
                CreatePet(player, _creature, 28085);
                        break;
            case 4: //cat
                CreatePet(player, _creature, 28097);
                        break;
            case 5: //carrion
                CreatePet(player, _creature, 26838);
                        break;
            case 6: //crab
                CreatePet(player, _creature, 24478);
                        break;   
            case 7: //crocolisk
                CreatePet(player, _creature, 1417);
                        break;  
            case 8: //gorila
                CreatePet(player, _creature, 28213);
                        break;
            case 9: //hound
                CreatePet(player, _creature, 29452);
                        break;
            case 10: //hynea
                CreatePet(player, _creature, 13036);
                        break;
            case 11: //Moth
                CreatePet(player, _creature, 27421);
                        break;
            case 12: //owl
                CreatePet(player, _creature, 23136);
                        break;
            case 13: //strider
                CreatePet(player, _creature, 22807);
                        break;
            case 414: //scorpid
                CreatePet(player, _creature, 9698);
                        break;
            case 15: //turtle
                CreatePet(player, _creature, 25482);
                        break;
        }
       
		
	   }
	   
	    return true;
	   
    }
	
  };
  
  	CreatureAI* GetAI(Creature* _creature) const override
	{
		return  new BeastmasterAI(_creature);
	}
	
};
 
void AddSC_Npc_Beastmaster()
{
    new Npc_Beastmaster();
}