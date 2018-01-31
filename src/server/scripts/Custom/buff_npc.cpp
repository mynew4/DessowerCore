/*
<--------------------------------------------------------------------------->
 - Developer(s): Ghostcrawler336
 - Edited: Synth
 - Updated 2017: beimax76
 - Complete: %100
 - ScriptName: 'Buff Master' 
<--------------------------------------------------------------------------->
*/

#include "ScriptPCH.h"
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
#include <sstream>
#include <string>

enum spells 
{
	POWER_WORD_FORTITUDE = 48162,
	PRAYER_OF_SPRITE = 48074,
	SHADOW_BUFF = 48170,
	KINGS_BUFF = 43223,
	ARCANE_BUFF = 36880,
	MARK_OF_THE_WILD = 48469,
	AEGIS_OF_NELTHARION = 51512,
	ELUNES_BLESSING = 26393,
	DALARAN_INTELECT = 61024
};

class buff_npc : public CreatureScript
{
	public:
		buff_npc() : CreatureScript("buff_npc"){}
		
	class buffAI : public ScriptedAI
	{
		public:
		buffAI(Creature* creature) : ScriptedAI(creature) {}
			
		void CreatureWhisperBasedOnBool(const char *text, Creature *creature, Player *player, bool value)
		{
			if (value)
			creature->TextEmote(text, player);
		}
			
		bool GossipHello(Player* player) override
		{
			return OnGossipHello(player, me);
		}
		
		bool OnGossipHello(Player * player, Creature * pCreature)
		{
			
			ClearGossipMenuFor(player);
			AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_paladin_blessedmending:35:35|tНаложи на меня заклинания!", GOSSIP_SENDER_MAIN, 1);
			AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Spy:35:35|tПока.", GOSSIP_SENDER_MAIN, 100);
			SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
			return true;
		}

		bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
		{
			uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
			uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
			return OnGossipSelect(player, me, sender, action);
		}

		bool OnGossipSelect(Player * player, Creature * creature, uint32 uiSender, uint32 uiAction)
		{
			
			ClearGossipMenuFor(player);
			
			if (uiSender == GOSSIP_SENDER_MAIN)
			{
				switch(uiAction)
				{
					case 1:
					
						player->CastSpell(player, POWER_WORD_FORTITUDE, true);
						player->CastSpell(player, KINGS_BUFF, true);
						player->CastSpell(player, MARK_OF_THE_WILD, true);
						player->CastSpell(player, PRAYER_OF_SPRITE, true);
						player->CastSpell(player, ARCANE_BUFF, true);
						player->CastSpell(player, SHADOW_BUFF, true);
						player->CastSpell(player, AEGIS_OF_NELTHARION, true);
						player->CastSpell(player, DALARAN_INTELECT, true);
						creature->TextEmote("Заклинания наложены.", player);
					break;

					case 100:
						CloseGossipMenuFor(player);
						break;
				}
			}
			return true;
		}
	};
	
	CreatureAI* GetAI(Creature* creature) const override
	{
		return  new buffAI(creature);
	}
};

void AddSC_buff_npc()
{
    new buff_npc();
}