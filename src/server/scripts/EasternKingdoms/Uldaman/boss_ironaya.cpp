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
SDName: Boss_Ironaya
SD%Complete: 100
SDComment:
SDCategory: Uldaman
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_ARCINGSMASH           8374
#define SPELL_KNOCKAWAY             10101
#define SPELL_WSTOMP                11876

class boss_ironaya : public CreatureScript
{
    public:
        boss_ironaya() : CreatureScript("boss_ironaya") {}

        struct boss_ironayaAI : public ScriptedAI
        {
            boss_ironayaAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 uiArcingTimer;
            bool bHasCastedWstomp;
            bool bHasCastedKnockaway;

            void Reset() override
            {
                uiArcingTimer = 3000;
                bHasCastedKnockaway = false;
                bHasCastedWstomp = false;
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(0);
            }

            void UpdateAI(uint32 uiDiff) override
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //If we are <50% hp do knockaway ONCE
                if (!bHasCastedKnockaway && HealthBelowPct(50))
                {
                    if (auto victim = me->getVictim())
                        DoCast(victim, SPELL_KNOCKAWAY, true);

                    // current aggro target is knocked away pick new target
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0))
                        me->TauntApply(target);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 1))
                        me->TauntApply(target);

                    //Shouldn't cast this agian
                    bHasCastedKnockaway = true;
                }

                //uiArcingTimer
                if (uiArcingTimer <= uiDiff)
                {
                    DoCast(SPELL_ARCINGSMASH);
                    uiArcingTimer = 13000;
                }
                else
                    uiArcingTimer -= uiDiff;

                if (!bHasCastedWstomp && HealthBelowPct(25))
                {
                    DoCast(SPELL_WSTOMP);
                    bHasCastedWstomp = true;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ironayaAI(creature);
        }
};

void AddSC_boss_ironaya()
{
    new boss_ironaya();
}
