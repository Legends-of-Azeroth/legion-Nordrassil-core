/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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

#include "ulduar.h"

enum Spells
{
    // Auriaya
    SPELL_SETINEL_BLAST                         = 64389,
    SPELL_SONIC_SCREECH                         = 64422,
    SPELL_TERRIFYING_SCREECH                    = 64386,
    SPELL_SUMMON_SWARMING_GUARDIAN              = 64396,
    SPELL_ACTIVATE_DEFENDER                     = 64449,
    SPELL_DEFENDER_TRIGGER                      = 64448,
    SPELL_SUMMON_DEFENDER                       = 64447,
    SPELL_BERSERK                               = 47008,
    // Feral Defender
    SPELL_FERAL_RUSH                            = 64496,
    SPELL_FERAL_POUNCE                          = 64478,
    SPELL_SEEPING_ESSENCE                       = 64458,
    SPELL_SUMMON_ESSENCE                        = 64457,
    SPELL_FERAL_ESSENCE                         = 64455,
    // Sanctum Sentry
    SPELL_SAVAGE_POUNCE                         = 64666,
    SPELL_RIP_FLESH                             = 64375,
    SPELL_STRENGHT_PACK                         = 64381
};

#define NPC_SANCTUM_SENTRY                      34014

// Achievements
#define ACHIEVEMENT_CRAZY_CAT_LADY              RAID_MODE(3006, 3007)
#define ACHIEVEMENT_NINE_LIVES                  RAID_MODE(3076, 3077)

enum Events
{
    EVENT_NONE,
    EVENT_SCREECH,
    EVENT_BLAST,
    EVENT_TERRIFYING,
    EVENT_SUMMON,
    EVENT_DEFENDER,
    EVENT_BERSERK
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY_1                                  = 1,
    SAY_DEATH                                   = 2,
    SAY_BERSERK                                 = 3,
    EMOTE_FEAR                                  = 4,
    EMOTE_DEFENDER                              = 5
};

enum Actions
{
    ACTION_CRAZY_CAT_LADY                       = 0,
    ACTION_NINE_LIVES                           = 1
};

class boss_auriaya : public CreatureScript
{
public:
    boss_auriaya() : CreatureScript("boss_auriaya") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_auriaya_AI (pCreature);
    }

    struct boss_auriaya_AI : public BossAI
    {
        boss_auriaya_AI(Creature *pCreature) : BossAI(pCreature, BOSS_AURIAYA)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        bool SentryDead;
        uint32 LivesCount;

        void Reset() override
        {
            _Reset();
        
            if (Creature* Sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                Sentry->GetMotionMaster()->MoveFollow(me, 0.5, 1.5, MOTION_SLOT_ACTIVE);
            if (Creature* Sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                Sentry->GetMotionMaster()->MoveFollow(me, 0.5, -1.5, MOTION_SLOT_ACTIVE);
        
            if (GetDifficultyID() == DIFFICULTY_25_N)
            {
                if (Creature* Sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                    Sentry->GetMotionMaster()->MoveFollow(me, -2.5, 1.5, MOTION_SLOT_ACTIVE);
                if (Creature* Sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                    Sentry->GetMotionMaster()->MoveFollow(me, -2.5, -1.5, MOTION_SLOT_ACTIVE);
            }
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void EnterCombat(Unit* who) override
        {
            if (!instance->CheckRequiredBosses(BOSS_AURIAYA, me->GetEntry(), who->ToPlayer()))
            {
                EnterEvadeMode();
                return;
            }

            _EnterCombat();
            Talk(SAY_AGGRO);
            SentryDead = false;
            LivesCount = 0;
            events.ScheduleEvent(EVENT_SCREECH, 60000);
            events.ScheduleEvent(EVENT_BLAST, 35500);
            events.ScheduleEvent(EVENT_TERRIFYING, 35000);
            events.ScheduleEvent(EVENT_DEFENDER, 65000);
            events.ScheduleEvent(EVENT_SUMMON, 120000);
            events.ScheduleEvent(EVENT_BERSERK, 600000);
        }

        void KilledUnit(Unit* who) override
        {
            if (!(rand()%5))
                Talk(SAY_SLAY_1);
        }

        void JustDied(Unit * /*victim*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);
        
            if (instance)
            {
                // Crazy Cat Lady
                if (!SentryDead)
                    instance->DoCompleteAchievement(ACHIEVEMENT_CRAZY_CAT_LADY);
                // Nine Lives
                if (LivesCount > 8)
                    instance->DoCompleteAchievement(ACHIEVEMENT_NINE_LIVES);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SCREECH:
                        DoCast(SPELL_SONIC_SCREECH);
                        events.ScheduleEvent(EVENT_SCREECH, 25000);
                        break;
                    case EVENT_TERRIFYING:
                        Talk(EMOTE_FEAR);
                        DoCast(SPELL_TERRIFYING_SCREECH);
                        events.ScheduleEvent(EVENT_TERRIFYING, 35000);
                        break;
                    case EVENT_BLAST:
                        DoCast(SPELL_SETINEL_BLAST);
                        events.ScheduleEvent(EVENT_BLAST, 35000);
                        break;
                    case EVENT_DEFENDER:
                        Talk(EMOTE_DEFENDER);
                        DoCast(SPELL_DEFENDER_TRIGGER);
                        DoCast(SPELL_ACTIVATE_DEFENDER);
                        events.CancelEvent(EVENT_DEFENDER);
                        break;
                    case EVENT_SUMMON:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60, true))
                            DoCast(pTarget, SPELL_SUMMON_SWARMING_GUARDIAN);
                        events.ScheduleEvent(EVENT_SUMMON, 35000);
                        break;
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK, true);
                        Talk(SAY_BERSERK);
                        events.CancelEvent(EVENT_BERSERK);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void DoAction(const int32 action) override
        {
            switch (action)
            {
                case ACTION_CRAZY_CAT_LADY:
                    SentryDead = true;
                    break;
                case ACTION_NINE_LIVES:
                    LivesCount++;
                    break;
            }
        }
    };

};


class npc_feral_defender_trigger : public CreatureScript
{
public:
    npc_feral_defender_trigger() : CreatureScript("npc_feral_defender_trigger") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_feral_defender_triggerAI (pCreature);
    }

    struct npc_feral_defender_triggerAI : public Scripted_NoMovementAI
    {
        npc_feral_defender_triggerAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            me->SetDisplayId(11686);
            me->setFaction(16);
        }

        uint32 SummonTimer;

        void Reset() override
        {
            SummonTimer = 3000;
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (!UpdateVictim())
                return;

            if (SummonTimer <= uiDiff)
            {
                DoCast(me, SPELL_SUMMON_DEFENDER);
                SummonTimer = 30000;
            } 
            else SummonTimer -= uiDiff;
        }

        void JustSummoned(Creature *summon) override
        {
            summon->AI()->DoZoneInCombat();
        }
    };

};


class npc_feral_defender : public CreatureScript
{
public:
    npc_feral_defender() : CreatureScript("npc_feral_defender") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_feral_defenderAI (pCreature);
    }

    struct npc_feral_defenderAI : public ScriptedAI
    {
        npc_feral_defenderAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 PounceTimer;
        uint32 RushTimer;
        uint32 RessTimer;
        uint32 Lifes;

        void Reset() override
        {
            PounceTimer = 5000;
            RushTimer = 12000;
            Lifes = 8;
            RessTimer = 999999;
        
            me->SetAuraStack(SPELL_FERAL_ESSENCE, me, Lifes);
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (!UpdateVictim())
                me->DespawnOrUnsummon();
            
            if (instance && instance->GetBossState(BOSS_AURIAYA) != IN_PROGRESS)
                me->DespawnOrUnsummon();

            if (PounceTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60, true))
                {
                    me->AddThreat(pTarget, 100.0f);
                    me->AI()->AttackStart(pTarget);
                    DoCast(pTarget, SPELL_FERAL_POUNCE);
                }
                PounceTimer = urand(15000, 20000);
            } 
            else PounceTimer -= uiDiff;
        
            if (RushTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60, true))
                {
                    me->AddThreat(pTarget, 100.0f);
                    me->AI()->AttackStart(pTarget);
                    DoCast(pTarget, SPELL_FERAL_RUSH);
                }
                RushTimer = urand(15000, 20000);
            } 
            else RushTimer -= uiDiff;
        
            if (RessTimer <= uiDiff)
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                for (uint8 i = 0; i < Lifes; ++i)
                    DoCast(me, SPELL_FERAL_ESSENCE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_AGGRESSIVE);
                RessTimer = 999999;
                DoZoneInCombat();
            } 
            else RessTimer -= uiDiff;
        
            DoMeleeAttackIfReady();
        }

        void DamageTaken(Unit* /*pKiller*/, uint32 &damage, DamageEffectType dmgType) override
        {
            if (damage >= me->GetHealth())
            {
                if (me->HasAura(SPELL_FERAL_ESSENCE))
                {
                    damage = 0;
                    DoCast(me, SPELL_SUMMON_ESSENCE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetFullHealth();
                    me->RemoveAllAuras();
                    me->AttackStop();
                    Lifes--;
                    PounceTimer = 35000;
                    RushTimer = 42000;
                    RessTimer = 30000;
                    if (instance)
                        if (Creature *pAuriaya = me->GetCreature(*me, instance->GetGuidData(DATA_AURIAYA)))
                            pAuriaya->AI()->DoAction(ACTION_NINE_LIVES);
                }
            }
        }
    };

};


class npc_sanctum_sentry : public CreatureScript
{
public:
    npc_sanctum_sentry() : CreatureScript("npc_sanctum_sentry") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_sanctum_sentryAI (pCreature);
    }

    struct npc_sanctum_sentryAI : public ScriptedAI
    {
        npc_sanctum_sentryAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 RipTimer;
        uint32 PounceTimer;
        uint32 CheckTimer;

        void Reset() override
        {
            RipTimer = urand(4000, 8000);
            PounceTimer = urand(20000, 30000);
            CheckTimer = 1000;
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (!UpdateVictim())
                return;

            if (RipTimer <= uiDiff)
            {
                DoCastVictim(SPELL_RIP_FLESH);
                RipTimer = urand(14000, 18000);
            }
            else RipTimer -= uiDiff;
        
            if (PounceTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60, true))
                {
                    me->AddThreat(pTarget, 100.0f);
                    me->AI()->AttackStart(pTarget);
                    DoCast(pTarget, SPELL_SAVAGE_POUNCE);
                }
                PounceTimer = urand(20000, 30000);
            }
            else PounceTimer -= uiDiff;
        
            // Increases the damage of all Sanctum Sentries within 10 yards by 30%
            if (CheckTimer < uiDiff)
            {
                uint8 aura = NULL;
                std::list<Creature*> Sanctum;
                GetCreatureListWithEntryInGrid(Sanctum, me, NPC_SANCTUM_SENTRY, 10.0f);
                for(std::list<Creature*>::iterator itr = Sanctum.begin(); itr != Sanctum.end(); ++itr)
                {
                    Creature *Sentry = *itr;

                    if (!Sentry)
                        continue;

                    if (Sentry->isAlive() && me != Sentry)
                        aura++;
                }

                if (aura)
                    me->SetAuraStack(SPELL_STRENGHT_PACK, me, aura);

                CheckTimer = 2000;
            }
            else CheckTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*victim*/) override
        {
            if (instance)
                if (Creature *pAuriaya = me->GetCreature(*me, instance->GetGuidData(DATA_AURIAYA)))
                    pAuriaya->AI()->DoAction(ACTION_CRAZY_CAT_LADY);
        }
    };

};


class npc_seeping_trigger : public CreatureScript
{
public:
    npc_seeping_trigger() : CreatureScript("npc_seeping_trigger") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_seeping_triggerAI (pCreature);
    }

    struct npc_seeping_triggerAI : public Scripted_NoMovementAI
    {
        npc_seeping_triggerAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            me->SetDisplayId(11686);
        }

        InstanceScript* instance;

        void Reset() override
        {
            DoCast(me, SPELL_SEEPING_ESSENCE);
        }
    
        void UpdateAI(uint32 uiDiff) override
        {
            if (instance && instance->GetBossState(BOSS_AURIAYA) != IN_PROGRESS)
                me->DespawnOrUnsummon();
        }
    };

};


void AddSC_boss_auriaya()
{
    new boss_auriaya();
    new npc_feral_defender_trigger();
    new npc_feral_defender();
    new npc_sanctum_sentry();
    new npc_seeping_trigger();
}
