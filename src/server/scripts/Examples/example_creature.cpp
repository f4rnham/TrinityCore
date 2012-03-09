/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Example_Creature
SD%Complete: 100
SDComment: Short custom scripting example
SDCategory: Script Examples
EndScriptData */

#include "ScriptPCH.h"

// **** This script is designed as an example for others to build on ****
// **** Please modify whatever you'd like to as this script is only for developement ****

// **** Script Info* ***
// This script is written in a way that it can be used for both friendly and hostile monsters
// Its primary purpose is to show just how much you can really do with scripts
// I recommend trying it out on both an agressive NPC and on friendly npc

// **** Quick Info* ***
// Functions with Handled Function marked above them are functions that are called automatically by the core
// Functions that are marked Custom Function are functions I've created to simplify code

enum Yells
{
    //List of text id's. The text is stored in database, also in a localized version
    //(if translation not exist for the textId, default english text will be used)
    //Not required to define in this way, but simplify if changes are needed.
    SAY_AGGRO                                   = -1999900,
    SAY_RANDOM_0                                = -1999901,
    SAY_RANDOM_1                                = -1999902,
    SAY_RANDOM_2                                = -1999903,
    SAY_RANDOM_3                                = -1999904,
    SAY_RANDOM_4                                = -1999905,
    SAY_BERSERK                                 = -1999906,
    SAY_PHASE                                   = -1999907,
    SAY_DANCE                                   = -1999908,
    SAY_SALUTE                                  = -1999909,
    SAY_EVADE                                   = -1999910,
};

enum Spells
{
    // List of spells.
    // Not required to define them in this way, but will make it easier to maintain in case spellId change
    SPELL_BUFF                                  = 25661,
    SPELL_ONE                                   = 12555,
    SPELL_ONE_ALT                               = 24099,
    SPELL_TWO                                   = 10017,
    SPELL_THREE                                 = 26027,
    SPELL_FRENZY                                = 23537,
    SPELL_BERSERK                               = 32965,
};

enum eEnums
{
    // any other constants
    FACTION_WORGEN                              = 24
};

//List of gossip item texts. Items will appear in the gossip window.
#define GOSSIP_ITEM     "I'm looking for a fight"

class example_creature : public CreatureScript
{
    public:

        example_creature()
            : CreatureScript("example_creature")
        {
        }

        struct example_creatureAI : public ScriptedAI
        {
            // *** HANDLED FUNCTION ***
            //This is the constructor, called only once when the Creature is first created
            example_creatureAI(Creature* c) : ScriptedAI(c) {}

            // *** CUSTOM VARIABLES ****
            //These variables are for use only by this individual script.
            //Nothing else will ever call them but us.

            uint32 m_uiSayTimer;                                    // Timer for random chat
            uint32 m_uiRebuffTimer;                                 // Timer for rebuffing
            uint32 m_uiSpell1Timer;                                 // Timer for spell 1 when in combat
            uint32 m_uiSpell2Timer;                                 // Timer for spell 1 when in combat
            uint32 m_uiSpell3Timer;                                 // Timer for spell 1 when in combat
            uint32 m_uiBeserkTimer;                                 // Timer until we go into Beserk (enraged) mode
            uint32 m_uiPhase;                                       // The current battle phase we are in
            uint32 m_uiPhaseTimer;                                  // Timer until phase transition

            // *** HANDLED FUNCTION ***
            //This is called after spawn and whenever the core decides we need to evade
            void Reset()
            {
                m_uiPhase = 1;                                      // Start in phase 1
                m_uiPhaseTimer = 60000;                             // 60 seconds
                m_uiSpell1Timer = 5000;                             //  5 seconds
                m_uiSpell2Timer = urand(10000, 20000);               // between 10 and 20 seconds
                m_uiSpell3Timer = 19000;                            // 19 seconds
                m_uiBeserkTimer = 120000;                           //  2 minutes

                me->RestoreFaction();
            }

            // *** HANDLED FUNCTION ***
            // Enter Combat called once per combat
            void EnterCombat(Unit* who)
            {
                //Say some stuff
                DoScriptText(SAY_AGGRO, me, who);
            }

            // *** HANDLED FUNCTION ***
            // Attack Start is called when victim change (including at start of combat)
            // By default, attack who and start movement toward the victim.
            //void AttackStart(Unit* who)
            //{
            //    ScriptedAI::AttackStart(who);
            //}

            // *** HANDLED FUNCTION ***
            // Called when going out of combat. Reset is called just after.
            void EnterEvadeMode()
            {
                DoScriptText(SAY_EVADE, me);
            }

            // *** HANDLED FUNCTION ***
            //Our Receive emote function
            void ReceiveEmote(Player* /*player*/, uint32 uiTextEmote)
            {
                me->HandleEmoteCommand(uiTextEmote);

                switch (uiTextEmote)
                {
                    case TEXT_EMOTE_DANCE:
                        DoScriptText(SAY_DANCE, me);
                        break;
                    case TEXT_EMOTE_SALUTE:
                        DoScriptText(SAY_SALUTE, me);
                        break;
                }
             }

            // *** HANDLED FUNCTION ***
            //Update AI is called Every single map update (roughly once every 50ms if a player is within the grid)
            void UpdateAI(const uint32 uiDiff)
            {
                //Out of combat timers
                if (!me->getVictim())
                {
                    //Random Say timer
                    if (m_uiSayTimer <= uiDiff)
                    {
                        //Random switch between 5 outcomes
                        DoScriptText(RAND(SAY_RANDOM_0, SAY_RANDOM_1, SAY_RANDOM_2, SAY_RANDOM_3, SAY_RANDOM_4), me);

                        m_uiSayTimer = 45000;                      //Say something agian in 45 seconds
                    }
                    else
                        m_uiSayTimer -= uiDiff;

                    //Rebuff timer
                    if (m_uiRebuffTimer <= uiDiff)
                    {
                        DoCast(me, SPELL_BUFF);
                        m_uiRebuffTimer = 900000;                  //Rebuff agian in 15 minutes
                    }
                    else
                        m_uiRebuffTimer -= uiDiff;
                }

                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //Spell 1 timer
                if (m_uiSpell1Timer <= uiDiff)
                {
                    //Cast spell one on our current target.
                    if (rand()%50 > 10)
                        DoCast(me->getVictim(), SPELL_ONE_ALT);
                    else if (me->IsWithinDist(me->getVictim(), 25.0f))
                        DoCast(me->getVictim(), SPELL_ONE);

                    m_uiSpell1Timer = 5000;
                }
                else
                    m_uiSpell1Timer -= uiDiff;

                //Spell 2 timer
                if (m_uiSpell2Timer <= uiDiff)
                {
                    //Cast spell two on our current target.
                    DoCast(me->getVictim(), SPELL_TWO);
                    m_uiSpell2Timer = 37000;
                }
                else
                    m_uiSpell2Timer -= uiDiff;

                //Beserk timer
                if (m_uiPhase > 1)
                {
                    //Spell 3 timer
                    if (m_uiSpell3Timer <= uiDiff)
                    {
                        //Cast spell one on our current target.
                        DoCast(me->getVictim(), SPELL_THREE);

                        m_uiSpell3Timer = 19000;
                    }
                    else
                        m_uiSpell3Timer -= uiDiff;

                    if (m_uiBeserkTimer <= uiDiff)
                    {
                        //Say our line then cast uber death spell
                        DoScriptText(SAY_BERSERK, me, me->getVictim());
                        DoCast(me->getVictim(), SPELL_BERSERK);

                        //Cast our beserk spell agian in 12 seconds if we didn't kill everyone
                        m_uiBeserkTimer = 12000;
                    }
                    else
                        m_uiBeserkTimer -= uiDiff;
                }
                else if (m_uiPhase == 1)                            //Phase timer
                {
                    if (m_uiPhaseTimer <= uiDiff)
                    {
                        //Go to next phase
                        ++m_uiPhase;
                        DoScriptText(SAY_PHASE, me);
                        DoCast(me, SPELL_FRENZY);
                    }
                    else
                        m_uiPhaseTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new example_creatureAI(creature);
        }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->CLOSE_GOSSIP_MENU();
                //Set our faction to hostile towards all
                creature->setFaction(FACTION_WORGEN);
                creature->AI()->AttackStart(player);
            }

            return true;
        }
};

enum train_destroyer_enum
{
    SUMMON_TRAIN = 61031,
    DESTROY_TRAIN = 62943,
    TRAIN = 193963,
};

class train_destroyer : public CreatureScript
{
    public:

        train_destroyer() : CreatureScript("train_destroyer") { }

        struct train_destroyerAI : public ScriptedAI
        {
            train_destroyerAI(Creature* c) : ScriptedAI(c) {}

            uint32 wait;
            uint8 phase;
            GameObject* train;
            uint64 trainID;
            Position trainPos;
            float dist;
            std::list <GameObject*> trains;

            void Reset()
            {
                phase = 0;
                wait = 0;
                trainID = 0;
            }

            void UpdateAI(const uint32 diff)
            {
                trains.clear();
                me->GetGameObjectListWithEntryInGrid(trains, TRAIN, 30.0);
                if (!trains.empty())
                {
                    HandleTrains();
                    if(trainID != train->GetGUID())
                    {
                        phase = 1;
                        wait = 0;
                    }
                }
                else
                {
                    phase = 0;
                    wait = 0;
                    return;
                }

                if (wait > diff)
                {
                    wait -= diff;
                    return;
                }

                switch (phase)
                {
                    case 1:
                        train->GetPosition(&trainPos);
                        //me->MovePosition(trainPos, -2, 0);
                        me->GetMotionMaster()->MovePoint( 0 , trainPos);
                        //me->MovePosition( trainPos, dist, 0);
                        trainID = train->GetGUID();
                        phase++;
                        break;
                    case 2:
                        if(!me->isMoving())
                            phase++;
                        break;
                    case 3:
                        me->GetMotionMaster()->MoveJump(train->GetPositionX(), train->GetPositionY(), train->GetPositionZ() , 1, 1);
                        phase++;
                        break;
                    case 4:
                        if(!me->isMoving())
                            phase++;
                        break;
                    case 5:
                        me->CastSpell(train, DESTROY_TRAIN, false);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_TRAIN);
                        phase++;
                        wait = 5000;
                        break;
                    case 6:
                        me->HandleEmoteCommand(EMOTE_STATE_DANCE);
                        phase++;
                        wait = 15000;
                    case 7:
                        me->DisappearAndDie();
                        train->Delete();
                        break;
                    default:
                        break;
                }
            };

            // choose closest one
            void HandleTrains()
            {
                train = trains.front();
                dist = me->GetDistance(train);
                for (std::list <GameObject*>::iterator it = trains.begin(); it != trains.end(); ++it)
                    if (me->GetDistance(*it) < dist)
                    {
                        dist = me->GetDistance(*it);
                        train = *it;
                    }
            };
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new train_destroyerAI(creature);
        }
};

//This is the actual function called only once durring InitScripts()
//It must define all handled functions that are to be run in this script
void AddSC_example_creature()
{
    new example_creature();
    new train_destroyer();
}
