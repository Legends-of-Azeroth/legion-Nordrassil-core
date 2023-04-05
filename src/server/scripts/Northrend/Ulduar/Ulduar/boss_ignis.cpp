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

/* ScriptData
SDName: Ignis the Furnace Master
SDAuthor: PrinceCreed
SD%Complete: 100
EndScriptData */

#include "ulduar.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"

enum Yells
{
    SAY_AGGRO                                   = -1603220,
    SAY_SLAY_1                                  = -1603221,
    SAY_SLAY_2                                  = -1603222,
    SAY_DEATH                                   = -1603223,
    SAY_SUMMON                                  = -1603224,
    SAY_SLAG_POT                                = -1603225,
    SAY_SCORCH_1                                = -1603226,
    SAY_SCORCH_2                                = -1603227,
    SAY_BERSERK                                 = -1603228,
    EMOTE_JETS                                  = -1603229
};

enum Spells
{
    // Ignis
    SPELL_FLAME_JETS                            = 62680,
    SPELL_SCORCH                                = 62546,
    SPELL_SLAG_POT                              = 62717,
    SPELL_SLAG_IMBUED_10                        = 62836,
    SPELL_SLAG_IMBUED_25                        = 63536,
    SPELL_SLAG_POT_DAMAGE                       = 65722,
    SPELL_ACTIVATE_CONSTRUCT                    = 62488,
    SPELL_STRENGHT                              = 64473,
    SPELL_GRAB                                  = 62707,
    SPELL_BERSERK                               = 47008,

    // Iron Construct
    SPELL_HEAT                                  = 65667,
    SPELL_MOLTEN                                = 62373,
    SPELL_BRITTLE                               = 62382,
    SPELL_SHATTER                               = 62383,
    SPELL_FREEZE_ANIM                           = 63354,

    // Scorch Ground
    SPELL_SCORCH_GROUND                         = 62548
};

enum Events
{
    EVENT_NONE,
    EVENT_JET,
    EVENT_SCORCH,
    EVENT_SLAG_POT,
    EVENT_GRAB_POT,
    EVENT_CHANGE_POT,
    EVENT_END_POT,
    EVENT_CONSTRUCT,
    EVENT_BERSERK
};

enum Npcs
{
    NPC_IRON_CONSTRUCT                          = 33121,
    NPC_SCORCH_GROUND                           = 33221
};

#define ACTION_REMOVE_BUFF                      1

enum Achievements
{
    ACHIEVEMENT_SHATTERED_10                    = 2925,
    ACHIEVEMENT_SHATTERED_25                    = 2926,
    ACHIEV_TIMED_START_EVENT                    = 20951,
};

const Position Pos[20] =
{
{630.366f,216.772f,360.891f,M_PI},
{630.594f,231.846f,360.891f,M_PI},
{630.435f,337.246f,360.886f,M_PI},
{630.493f,313.349f,360.886f,M_PI},
{630.444f,321.406f,360.886f,M_PI},
{630.366f,247.307f,360.888f,M_PI},
{630.698f,305.311f,360.886f,M_PI},
{630.500f,224.559f,360.891f,M_PI},
{630.668f,239.840f,360.890f,M_PI},
{630.384f,329.585f,360.886f,M_PI},
{543.220f,313.451f,360.886f,0},
{543.356f,329.408f,360.886f,0},
{543.076f,247.458f,360.888f,0},
{543.117f,232.082f,360.891f,0},
{543.161f,305.956f,360.886f,0},
{543.277f,321.482f,360.886f,0},
{543.316f,337.468f,360.886f,0},
{543.280f,239.674f,360.890f,0},
{543.265f,217.147f,360.891f,0},
{543.256f,224.831f,360.891f,0}
};


class boss_ignis : public CreatureScript
{
public:
    boss_ignis() : CreatureScript("boss_ignis") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_ignis_AI (pCreature);
    }

    struct boss_ignis_AI : public BossAI
    {
        boss_ignis_AI(Creature *pCreature) : BossAI(pCreature, BOSS_IGNIS), vehicle(me->GetVehicleKit())
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        Vehicle* vehicle;
        std::vector<ObjectGuid> construct_list;
    
        ObjectGuid SlagPotGUID;
        uint32 ConstructTimer;
        uint8 ConstructVal;
        bool Shattered;

        void Reset() override
        {
            _Reset();
            events.ScheduleEvent(EVENT_JET, 30000);
            events.ScheduleEvent(EVENT_SCORCH, 25000);
            events.ScheduleEvent(EVENT_SLAG_POT, 35000);
            events.ScheduleEvent(EVENT_CONSTRUCT, 15000);
            events.ScheduleEvent(EVENT_END_POT, 40000);
            events.ScheduleEvent(EVENT_BERSERK, 480000);
            me->SetReactState(REACT_DEFENSIVE);
            ConstructVal = 0;
            SlagPotGUID.Clear();
            ConstructTimer = 0;
            Shattered = false;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (instance)
            {
                instance->DoStopTimedAchievement(CRITERIA_TIMED_TYPE_EVENT2, ACHIEV_TIMED_START_EVENT);
            }
            
            construct_list.clear();
        
            if (vehicle)
                vehicle->RemoveAllPassengers();
            
            for (uint8 n = 0; n < 20; n++)
            {
                if (Creature* Construct = me->SummonCreature(NPC_IRON_CONSTRUCT, Pos[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                    construct_list.push_back(Construct->GetGUID());
            }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            DoScriptText(SAY_AGGRO, me);
            SetCombatMovement(true);

            // Stokin' the Furnace
            if (instance)
                instance->DoStartTimedAchievement(CRITERIA_TIMED_TYPE_EVENT2, ACHIEV_TIMED_START_EVENT);
        }

        void JustDied(Unit* /*victim*/) override
        {
            _JustDied();
            DoScriptText(SAY_DEATH, me);
        }
        
        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (ConstructTimer)
            {
                if (ConstructTimer <= diff)
                {
                    if (ConstructVal >= 2)
                    {
                        ConstructTimer = 0;
                        ConstructVal = 0;
                        Shattered = true;
                        return;
                    }
                    else
                    {
                        ConstructTimer = 0;
                        ConstructVal = 0;
                    }
                } 
                else ConstructTimer -= diff;

            }
                
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_JET:
                        me->MonsterTextEmote(EMOTE_JETS, ObjectGuid::Empty, true);
                        DoCastAOE(SPELL_FLAME_JETS);
                        events.ScheduleEvent(EVENT_JET, urand(35000,40000));
                        break;
                    case EVENT_SLAG_POT:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                        {
                            DoScriptText(SAY_SLAG_POT, me);
                            SlagPotGUID = pTarget->GetGUID();
                            DoCast(pTarget, SPELL_GRAB);
                            events.DelayEvents(3000);
                            events.ScheduleEvent(EVENT_GRAB_POT, 500);
                        }
                        events.ScheduleEvent(EVENT_SLAG_POT, RAID_MODE(30000, 15000));
                        break;
                    case EVENT_GRAB_POT:
                        if (Unit* SlagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
                        {
                            SlagPotTarget->EnterVehicle(me, 0);
                            events.ScheduleEvent(EVENT_CHANGE_POT, 1000);
                        }
                        break;
                    case EVENT_CHANGE_POT:
                        if (Unit* SlagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
                        {
                            me->CastSpell(SlagPotTarget, SPELL_SLAG_POT, true);
                            SlagPotTarget->ChangeSeat(1);
                            events.ScheduleEvent(EVENT_END_POT, 10000);
                        }
                        break;
                    case EVENT_END_POT:
                        if (Unit* SlagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
                        {
                            SlagPotTarget->ExitVehicle();
                            SlagPotTarget = NULL;
                            SlagPotGUID.Clear();
                        }
                        break;
                    case EVENT_SCORCH:
                        DoScriptText(RAND(SAY_SCORCH_1, SAY_SCORCH_2), me);
                        if (Unit *pTarget = me->getVictim())
                            me->SummonCreature(NPC_SCORCH_GROUND,pTarget->GetPositionX(),pTarget->GetPositionY(),pTarget->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,45000);
                        DoCast(SPELL_SCORCH);
                        events.ScheduleEvent(EVENT_SCORCH, 25000);
                        break;
                    case EVENT_CONSTRUCT:
                        if (!construct_list.empty())
                        {
                            std::vector<ObjectGuid>::iterator itr = (construct_list.begin()+rand()%construct_list.size());
                            if (Creature* pTarget = me->GetMap()->GetCreature(*itr))
                            {
                                DoScriptText(SAY_SUMMON, me);
                                DoCast(me, SPELL_STRENGHT, true);
                                DoCast(me, SPELL_ACTIVATE_CONSTRUCT);
                                pTarget->setFaction(16);
                                pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                construct_list.erase(itr);
                            }
                        }
                        events.ScheduleEvent(EVENT_CONSTRUCT, RAID_MODE(40000, 30000));
                        break;
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK, true);
                        DoScriptText(SAY_BERSERK, me);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit* who) override
        {
            if (!(rand()%5))
                DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        
        void DoAction(const int32 action) override
        {
            switch(action)
            {
               case ACTION_REMOVE_BUFF:
                   me->RemoveAuraFromStack(SPELL_STRENGHT);
                   if (!ConstructTimer)
                       ConstructTimer = 5000;
                   ConstructVal++;
                   break;
            }
        }

        bool isShattered()
        {
            return Shattered;
        }
    };
};


class npc_iron_construct : public CreatureScript
{
public:
    npc_iron_construct() : CreatureScript("npc_iron_construct") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_iron_constructAI (pCreature);
    }

    struct npc_iron_constructAI : public ScriptedAI
    {
        npc_iron_constructAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_PASSIVE);
            me->AddAura(SPELL_FREEZE_ANIM, me);
            me->setFaction(35);
        }

        InstanceScript *instance;

        bool Brittled;
        
        void Reset() override
        { 
            Brittled = false;
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage, DamageEffectType dmgType) override
        {
            if (me->HasAura(SPELL_BRITTLE) && damage >= 5000)
            {
                DoCastAOE(SPELL_SHATTER, true);
                if (Creature *pIgnis = me->GetCreature(*me, instance->GetGuidData(DATA_IGNIS)))
                        pIgnis->AI()->DoAction(ACTION_REMOVE_BUFF);
                me->DespawnOrUnsummon(1000);
            }
        }
    
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_ACTIVATE_CONSTRUCT && me->HasReactState(REACT_PASSIVE))
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                me->AI()->AttackStart(caster->getVictim());
                me->AI()->DoZoneInCombat();
            }
        }

        void UpdateAI(uint32 uiDiff) override
        {
            Map *cMap = me->GetMap();

            if (me->HasAura(SPELL_MOLTEN) && me->HasAura(SPELL_HEAT))
                me->RemoveAura(SPELL_HEAT);

            if (Aura * aur = me->GetAura((SPELL_HEAT)))
            {
                if (aur->GetStackAmount() >= 10)
                {
                    me->RemoveAura(SPELL_HEAT);
                    DoCast(me, SPELL_MOLTEN, true);
                    Brittled = false;
                }
            }

            // Water pools
            if (me->IsInWater() && !Brittled && me->HasAura(SPELL_MOLTEN))
            {
                me->AddAura(SPELL_BRITTLE, me);
                me->RemoveAura(SPELL_MOLTEN);
                Brittled = true;
            }

            DoMeleeAttackIfReady();
        }
    };

};


class npc_scorch_ground : public CreatureScript
{
public:
    npc_scorch_ground() : CreatureScript("npc_scorch_ground") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_scorch_groundAI (pCreature);
    }

    struct npc_scorch_groundAI : public Scripted_NoMovementAI
    {
        npc_scorch_groundAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
            me->SetDisplayId(16925);
        }

        void Reset() override
        {
            DoCast(me, SPELL_SCORCH_GROUND);
        }
    };

};


class spell_ignis_slag_pot : public SpellScriptLoader
{
    public:
        spell_ignis_slag_pot() : SpellScriptLoader("spell_ignis_slag_pot") { }
 
        class spell_ignis_slag_pot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ignis_slag_pot_AuraScript)
            bool Validate(SpellInfo const * /*SpellInfo*/) override
            {
                if (!sSpellStore.LookupEntry(SPELL_SLAG_POT_DAMAGE))
                    return false;
                if (!sSpellStore.LookupEntry(SPELL_SLAG_POT))
                    return false;
                if (!sSpellStore.LookupEntry(SPELL_SLAG_IMBUED_10))
                    return false;
                if (!sSpellStore.LookupEntry(SPELL_SLAG_IMBUED_25))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const * aurEff)
            {
                Unit* aurEffCaster = aurEff->GetCaster();
                if (!aurEffCaster)
                    return;

                Unit * target = GetTarget();
                aurEffCaster->CastSpell(target, SPELL_SLAG_POT_DAMAGE, true);
                if (target->isAlive() && !GetDuration())
                {
                    if (aurEffCaster->GetMap()->GetDifficultyID() == DIFFICULTY_10_N)
                        target->CastSpell(target, SPELL_SLAG_IMBUED_10, true);
                    else if (aurEffCaster->GetMap()->GetDifficultyID() == DIFFICULTY_25_N)
                        target->CastSpell(target, SPELL_SLAG_IMBUED_25, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ignis_slag_pot_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ignis_slag_pot_AuraScript();
        }
};

class achievement_shattered : public AchievementCriteriaScript
{
    public:
        achievement_shattered() : AchievementCriteriaScript("achievement_shattered")
        {
        }

        bool OnCheck(Player* player, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature * ignis = target->ToCreature())
                if (boss_ignis::boss_ignis_AI* ignisAI = CAST_AI(boss_ignis::boss_ignis_AI, ignis->AI()))
                    if (ignisAI->isShattered())
                        return true;

            return false;
        }
};

void AddSC_boss_ignis()
{
    new boss_ignis();
    new npc_iron_construct();
    new npc_scorch_ground();
    new spell_ignis_slag_pot();
    new achievement_shattered();

    if (VehicleSeatEntry* vehSeat = const_cast<VehicleSeatEntry*>(sVehicleSeatStore.LookupEntry(3206)))
        vehSeat->Flags |= 0x400;
}