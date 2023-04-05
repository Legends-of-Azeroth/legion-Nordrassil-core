#include "zulaman.h"

enum ScriptTexts
{
    SAY_AGGRO       = 0,
    SAY_FIRE_BOMB   = 1,
    SAY_HATCHER     = 2,
    SAY_35          = 3,
    SAY_KILL        = 4,
    SAY_DEATH       = 5,
};

enum Spells
{
    SPELL_FLAME_BREATH          = 43140,
    SPELL_FLAME_BREATH_1        = 97497,
    SPELL_FIRE_WALL             = 43113,
    SPELL_HATCH_EGG             = 42471,
    SPELL_HATCH_ALL             = 43144,
    SPELL_FIRE_BOMB_CHANNEL     = 42621,
    SPELL_FIRE_BOMB_THROW       = 42628,
    SPELL_FIRE_BOMB_DUMMY       = 42629,
    SPELL_FIRE_BOMB_DAMAGE      = 42630,
    SPELL_TELE_TO_CENTER        = 43098,
    SPELL_SUMMON_HATCHLING      = 42493,
    SPELL_FRENZY                = 44779,
    SPELL_FLAMEBUFFET           = 43299,
    SPELL_SUMMON_PLAYERS        = 43097,
};

enum Adds
{
    NPC_FIRE_BOMB           = 23920,
    NPC_AMANISHI_HATCHER1   = 23818,
    NPC_AMANISHI_HATCHER2   = 24504,
    NPC_EGG                 = 23817,
    NPC_HATCHLING           = 23598,
    NPC_WORLD_TRIGGER       = 21252,
};

enum Events
{
    EVENT_FLAME_BREATH      = 1,
    EVENT_CONTINUE          = 2,
    EVENT_SUMMON_HATCHERS   = 3,
    EVENT_SPAWN_BOMBS       = 4,
    EVENT_SUMMON_BOMBS      = 5,
    EVENT_DETONATE_BOMBS    = 6,
    EVENT_TELEPORT          = 7,
    EVENT_FLAMEBUFFET       = 8,
};

const int area_dx = 44;
const int area_dy = 51;

const Position posJanalai = {-33.93f, 1149.27f, 19.0f, 0.0f};

const Position posFireWall[4] = 
{
    {-33.89f, 1122.81f, 18.80f, 1.58f},
    {-10.28f, 1149.97f, 18.80f, 3.14f},
    {-33.43f, 1177.72f, 18.80f, 4.66f},
    {-53.62f, 1150.03f, 18.80f, 0.00f}
};

const Position posHatchersWay[2][5] = 
{
    {
        {-87.46f, 1170.09f, 6.0f, 0.0f},
        {-74.41f, 1154.75f, 6.0f, 0.0f},
        {-52.74f, 1153.32f, 19.0f, 0.0f},
        {-33.37f, 1172.46f, 19.0f, 0.0f},
        {-33.09f, 1203.87f, 19.0f, 0.0f}
    },
    {
        {-86.57f, 1132.85f, 6.0f, 0.0f},
        {-73.94f, 1146.00f, 6.0f, 0.0f},
        {-52.29f, 1146.51f, 19.0f, 0.0f},
        {-33.57f, 1125.72f, 19.0f, 0.0f},
        {-34.29f, 1095.22f, 19.0f, 0.0f}
    }
};

const Position posEggs[36] =
{
    {-41.177f, 1084.59f, 18.7948f, 1.06465f},
    {-33.6638f, 1087.02f, 18.7948f, 0.959931f},
    {-36.2434f, 1088.15f, 18.7948f, 1.72788f},
    {-31.0391f, 1088.33f, 18.7948f, 2.70526f},
    {-35.0347f, 1084.92f, 18.7948f, 5.21853f},
    {-28.0851f, 1214.22f, 18.7947f, 3.38594f},
    {-27.0043f, 1211.99f, 18.7947f, 3.94444f},
    {-29.8651f, 1211.38f, 18.7947f, 2.94961f},
    {-29.1757f, 1090.27f, 18.7948f, 0.680678f},
    {-40.7069f, 1088.51f, 18.7948f, 0.0174533f},
    {-34.4183f, 1213.35f, 18.7947f, 2.26893f},
    {-32.7619f, 1215.33f, 18.7947f, 2.80998f},
    {-36.2872f, 1218.1f, 18.7947f, 0.0349066f},
    {-38.5764f, 1218.68f, 18.7947f, 4.97419f},
    {-26.5745f, 1084.44f, 18.7948f, 2.79253f},
    {-40.0005f, 1090.55f, 18.7948f, 1.11701f},
    {-38.2802f, 1088.14f, 18.7948f, 1.27409f},
    {-37.3368f, 1212.53f, 18.7947f, 0.314159f},
    {-36.4398f, 1209.93f, 18.7947f, 0.331613f},
    {-39.3636f, 1209.73f, 18.7947f, 0.593412f},
    {-39.7272f, 1216.09f, 18.7947f, 5.58505f},
    {-29.7244f, 1208.43f, 18.7947f, 4.93928f},
    {-38.9577f, 1207.25f, 18.7947f, 4.06662f},
    {-34.0586f, 1207.23f, 18.7947f, 4.60767f},
    {-28.0705f, 1216.81f, 18.7947f, 1.39626f},
    {-30.4305f, 1216.39f, 18.7947f, 4.90438f},
    {-32.0784f, 1218.55f, 18.7947f, 5.65487f},
    {-33.1212f, 1209.77f, 18.7947f, 2.77507f},
    {-28.4201f, 1082.09f, 18.7948f, 4.01426f},
    {-30.5147f, 1084.72f, 18.7948f, 1.79769f},
    {-34.0568f, 1082.02f, 18.7947f, 2.67035f},
    {-31.6647f, 1081.88f, 18.7948f, 6.17846f},
    {-38.8813f, 1084.2f, 18.7948f, 0.575959f},
    {-33.5926f, 1090.16f, 18.7948f, 5.13127f},
    {-42.8135f, 1085.94f, 18.7948f, 2.04204f},
    {-39.7956f, 1081.47f, 18.7948f, 2.74017f}
};

class boss_janalai : public CreatureScript
{
    public:

        boss_janalai() : CreatureScript("boss_janalai") {}
        
        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<boss_janalaiAI>(pCreature);
        }

        struct boss_janalaiAI : public BossAI
        {
            boss_janalaiAI(Creature* pCreature) : BossAI(pCreature, DATA_JANALAI)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            bool bEnraged;

            uint8 bombsCount;
            ObjectGuid FireBombsGUID[40];

            void Reset()
            {
                _Reset();

                for (uint8 i = 0; i < 36; ++i)
                    me->SummonCreature(NPC_EGG, posEggs[i], TEMPSUMMON_DEAD_DESPAWN);

                memset(&FireBombsGUID, 0, sizeof(FireBombsGUID));
                bombsCount = 0;
                bEnraged = false;
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                Talk(SAY_KILL);
            }

            void EnterCombat(Unit* /*who*/)
            {
                bEnraged = false;
                Talk(SAY_AGGRO);
                events.RescheduleEvent(EVENT_FLAME_BREATH, 7000);
                events.RescheduleEvent(EVENT_SUMMON_HATCHERS, 13000);
                events.RescheduleEvent(EVENT_TELEPORT, 55000);
                DoZoneInCombat();
                instance->SetBossState(DATA_JANALAI, IN_PROGRESS);
            }

            void Firewall()
            {
                for (uint8 i = 0; i < 4; i++)
                {
                    if (Creature* pTrigger = me->SummonCreature(NPC_WORLD_TRIGGER, posFireWall[i], TEMPSUMMON_TIMED_DESPAWN, 12000))
                        pTrigger->CastSpell(pTrigger, SPELL_FIRE_WALL, true);
                }
            }

            void SpawnBombs()
            {
                float dx, dy;
                for (uint8 i = 0; i < 40; ++i)
                {
                    dx = float(irand(-area_dx / 2, area_dx / 2));
                    dy = float(irand(-area_dy / 2, area_dy / 2));

                    if (Creature* pBomb = DoSpawnCreature(NPC_FIRE_BOMB, dx, dy, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
                        FireBombsGUID[i] = pBomb->GetGUID();
                }
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                EnterEvadeIfOutOfCombatArea(diff);
                
                if (me->HealthBelowPct(35) && !bEnraged)
                {
                    bEnraged = true;
                    Talk(SAY_35);
                    DoCast(me, SPELL_FRENZY, true);
                    events.CancelEvent(EVENT_SUMMON_HATCHERS);
                    DoCast(me, SPELL_HATCH_ALL);
                    return;
                }

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                     switch (eventId)
                     {
                        case EVENT_FLAME_BREATH:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(pTarget, SPELL_FLAME_BREATH);
                            events.RescheduleEvent(EVENT_CONTINUE, 3000);
                            events.RescheduleEvent(EVENT_FLAME_BREATH, 9000);
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->getVictim());
                            break;
                        case EVENT_SUMMON_HATCHERS:
                            Talk(SAY_HATCHER);
                            if (!summons.HasEntry(NPC_AMANISHI_HATCHER1))
                                me->SummonCreature(NPC_AMANISHI_HATCHER1, posHatchersWay[0][0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                            if (!summons.HasEntry(NPC_AMANISHI_HATCHER2))
                                me->SummonCreature(NPC_AMANISHI_HATCHER2, posHatchersWay[1][0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                            events.RescheduleEvent(EVENT_SUMMON_HATCHERS, 90000);
                            break;
                        case EVENT_SPAWN_BOMBS:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            bombsCount = 0;
                            Talk(SAY_FIRE_BOMB);
                            SpawnBombs();
                            events.RescheduleEvent(EVENT_SUMMON_BOMBS, 1000);
                            break;
                        case EVENT_SUMMON_BOMBS:
                            if (bombsCount < 40)
                            {
                                if (Creature* pBomb = Creature::GetCreature(*me, FireBombsGUID[bombsCount]))
                                    DoCast(pBomb, SPELL_FIRE_BOMB_THROW, true);
                                bombsCount++;
                                events.RescheduleEvent(EVENT_SUMMON_BOMBS, 100);
                            }
                            else
                                events.RescheduleEvent(EVENT_DETONATE_BOMBS, 2000);
                            break;
                        case EVENT_DETONATE_BOMBS:
                            for (uint8 i = 0; i < 40; ++i)
                                if (Creature* pBomb = Creature::GetCreature(*me, FireBombsGUID[i]))
                                {
                                    pBomb->RemoveAllAuras();
                                    pBomb->CastSpell(pBomb, SPELL_FIRE_BOMB_DAMAGE, true);
                                }
                            events.RescheduleEvent(EVENT_CONTINUE, 1000);
                            break;
                        case EVENT_TELEPORT:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            events.RescheduleEvent(EVENT_SUMMON_HATCHERS, events.GetNextEventTime(EVENT_SUMMON_HATCHERS) + 7000);
                            events.RescheduleEvent(EVENT_FLAME_BREATH, events.GetNextEventTime(EVENT_FLAME_BREATH) + 7000);
                            Firewall();
                            DoCast(me, SPELL_TELE_TO_CENTER, true);
                            events.RescheduleEvent(EVENT_SPAWN_BOMBS, 2000);
                            break;

                     }
                }

                DoMeleeAttackIfReady();
            }
        };
};

class npc_janalai_hatcher : public CreatureScript
{
    public:
        npc_janalai_hatcher() : CreatureScript("npc_janalai_hatcher") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_janalai_hatcherAI>(pCreature);
        }

        struct npc_janalai_hatcherAI : public ScriptedAI
        {
            npc_janalai_hatcherAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetSpeed(MOVE_RUN, 0.8f);
                me->SetReactState(REACT_PASSIVE);
                instance = pCreature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 waypoint;
            uint32 WaitTimer;

            bool side;
            bool hasChangedSide;
            bool isHatching;

            void Reset()
            {
                side =(me->GetEntry() ==  24504);
                waypoint = 0;
                isHatching = false;
                hasChangedSide = false;
                WaitTimer = 1;
            }
            void MovementInform(uint32, uint32)
            {
                if (waypoint == 5)
                {
                    isHatching = true;
                    WaitTimer = 5000;
                }
                else
                    WaitTimer = 1;
            }

            void UpdateAI(uint32 diff)
            {
                if (!instance || !(instance->GetBossState(DATA_JANALAI) == IN_PROGRESS))
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!isHatching)
                {
                    if (WaitTimer)
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, posHatchersWay[side][waypoint]);
                        ++waypoint;
                        WaitTimer = 0;
                    }
                }
                else
                {
                    if (WaitTimer <= diff)
                    {
                        if (me->FindNearestCreature(NPC_EGG, 50.0f))
                        {
                            DoCast(SPELL_HATCH_EGG);
                            WaitTimer = 6000;
                        }
                        else if (!hasChangedSide)
                        {
                            side = side ? 0 : 1;
                            isHatching = false;
                            waypoint = 3;
                            WaitTimer = 1;
                            hasChangedSide = true;
                        }
                        else
                            me->DespawnOrUnsummon();

                    } else WaitTimer -= diff;
                }
            }
        };
};

class npc_janalai_egg : public CreatureScript
{
    public:
        npc_janalai_egg(): CreatureScript("npc_janalai_egg") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_janalai_eggAI>(pCreature);
        }

        struct npc_janalai_eggAI : public ScriptedAI
        {
            npc_janalai_eggAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_HATCH_EGG || spell->Id == SPELL_HATCH_ALL)
                {
                    DoCast(SPELL_SUMMON_HATCHLING);
                    me->DespawnOrUnsummon();
                }
            }
        };
};

class npc_janalai_firebomb : public CreatureScript
{
    public:
        npc_janalai_firebomb() : CreatureScript("npc_janalai_firebomb") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_janalai_firebombAI>(pCreature);
        }

        struct npc_janalai_firebombAI : public ScriptedAI
        {
            npc_janalai_firebombAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_FIRE_BOMB_THROW)
                {
                    DoCast(me, SPELL_FIRE_BOMB_DUMMY, true);
                }
            }
      };
};

class npc_janalai_hatchling : public CreatureScript
{
    public:
        npc_janalai_hatchling() : CreatureScript("npc_janalai_hatchling")  {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return GetInstanceAI<npc_janalai_hatchlingAI>(pCreature);
        }

        struct npc_janalai_hatchlingAI : public ScriptedAI
        {
            npc_janalai_hatchlingAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                instance = pCreature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;

            void Reset()
            {
                events.Reset();
                if (me->GetPositionY() > 1150)
                    me->GetMotionMaster()->MovePoint(0, posHatchersWay[0][3].GetPositionX()+rand()%4-2, 1150.0f+rand()%4-2, posHatchersWay[0][3].GetPositionZ());
                else
                    me->GetMotionMaster()->MovePoint(0, posHatchersWay[1][3].GetPositionX()+rand()%4-2, 1150.0f+rand()%4-2, posHatchersWay[1][3].GetPositionZ());

                //me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
            }

            void EnterCombat(Unit* /*who*/) 
            {
                events.RescheduleEvent(EVENT_FLAMEBUFFET, urand(7000, 15000));
            }

            void UpdateAI(uint32 diff)
            {
                if (!instance || !(instance->GetBossState(DATA_JANALAI) == IN_PROGRESS))
                {
                    me->DisappearAndDie();
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                     switch (eventId)
                     {
                        case EVENT_FLAMEBUFFET:
                            DoCastVictim(SPELL_FLAMEBUFFET);
                            break;
                     }
                }

                DoMeleeAttackIfReady();
            }
        };
};

class spell_janalai_flame_breath : public SpellScriptLoader
{
    public:
        spell_janalai_flame_breath() : SpellScriptLoader("spell_janalai_flame_breath") { }

        class spell_janalai_flame_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_janalai_flame_breath_AuraScript);
            
            bool Load()
            {
                count = 0;
                return true;
            }

            void PeriodicTick(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;

                count++;
                Position pos;
                GetCaster()->GetNearPosition(pos, 4.0f * count, 0.0f);
                GetCaster()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_FLAME_BREATH_1, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_janalai_flame_breath_AuraScript::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            uint8 count;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_janalai_flame_breath_AuraScript();
        }
};

void AddSC_boss_janalai()
{
    new boss_janalai();
    new npc_janalai_firebomb();
    new npc_janalai_hatcher();
    new npc_janalai_hatchling();
    new npc_janalai_egg();
    new spell_janalai_flame_breath();
}

