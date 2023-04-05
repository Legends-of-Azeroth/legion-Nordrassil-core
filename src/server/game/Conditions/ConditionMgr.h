/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#ifndef TRINITY_CONDITIONMGR_H
#define TRINITY_CONDITIONMGR_H

#include "LootMgr.h"
#include "Errors.h"

struct PlayerConditionEntry;
class Player;
class Unit;
class WorldObject;
class LootTemplate;
struct Condition;

enum ConditionTypes
{                                                           // value1           value2         value3
    CONDITION_NONE                  = 0,                    // 0                0              0                  always true
    CONDITION_AURA                  = 1,                    // spell_id         effindex       use target?        true if player (or target, if value3) has aura of spell_id with effect effindex
    CONDITION_ITEM                  = 2,                    // item_id          count          bank               true if has #count of item_ids (if 'bank' is set it searches in bank slots too)
    CONDITION_ITEM_EQUIPPED         = 3,                    // item_id          0              0                  true if has item_id equipped
    CONDITION_ZONEID                = 4,                    // zone_id          0              0                  true if in zone_id
    CONDITION_REPUTATION_RANK       = 5,                    // faction_id       rankMask       0                  true if has min_rank for faction_id
    CONDITION_TEAM                  = 6,                    // player_team      0,             0                  469 - Alliance, 67 - Horde)
    CONDITION_SKILL                 = 7,                    // skill_id         skill_value    0                  true if has skill_value for skill_id
    CONDITION_QUESTREWARDED         = 8,                    // quest_id         0              0                  true if quest_id was rewarded before
    CONDITION_QUESTTAKEN            = 9,                    // quest_id         0,             0                  true while quest active
    CONDITION_DRUNKENSTATE          = 10,                   // DrunkenState     0,             0                  true if player is drunk enough
    CONDITION_WORLD_STATE           = 11,                   // index            value          0                  true if world has the value for the index
    CONDITION_ACTIVE_EVENT          = 12,                   // event_id         0              0                  true if event is active
    CONDITION_INSTANCE_INFO         = 13,                   // entry            data           0                  true if data is set in current instance
    CONDITION_QUEST_NONE            = 14,                   // quest_id         0              0                  true if doesn't have quest saved
    CONDITION_CLASS                 = 15,                   // class            0              0                  true if player's class is equal to class
    CONDITION_RACE                  = 16,                   // race             0              0                  true if player's race is equal to race
    CONDITION_ACHIEVEMENT           = 17,                   // achievement_id   0              0                  true if achievement is complete
    CONDITION_TITLE                 = 18,                   // title id         0              0                  true if player has title
    CONDITION_SPAWNMASK             = 19,                   // spawnMask        0              0
    CONDITION_GENDER                = 20,                   // gender           0              0                  true if player's gender is equal to gender
    CONDITION_UNIT_STATE            = 21,                   // unitState        0              0                  true if unit has unitState
    CONDITION_MAPID                 = 22,                   // map_id           0              0                  true if in map_id
    CONDITION_AREAID                = 23,                   // area_id          0              0                  true if in area_id
    CONDITION_CREATURE_TYPE         = 24,                   // cinfo.type       0              0                  true if creature_template.type = value1
    CONDITION_SPELL                 = 25,                   // spell_id         0              0                  true if player has learned spell
    CONDITION_PHASEID               = 26,                   // phaseid          0              0                  true if object is in phaseid
    CONDITION_LEVEL                 = 27,                   // level            ComparisonType 0                  true if unit's level is equal to param1 (param2 can modify the statement)
    CONDITION_QUEST_COMPLETE        = 28,                   // quest_id         0              0                  true if player has quest_id with all objectives complete, but not yet rewarded
    CONDITION_NEAR_CREATURE         = 29,                   // creature entry   distance       0                  true if there is a creature of entry in range
    CONDITION_NEAR_GAMEOBJECT       = 30,                   // gameobject entry distance       0                  true if there is a gameobject of entry in range
    CONDITION_OBJECT_ENTRY_GUID     = 31,                   // TypeID           entry          guid               true if object is type TypeID and the entry is 0 or matches entry of the object or matches guid of the object
    CONDITION_TYPE_MASK             = 32,                   // TypeMask         0              0                  true if object is type object's TypeMask matches provided TypeMask
    CONDITION_RELATION_TO           = 33,                   // ConditionTarget  RelationType   0                  true if object is in given relation with object specified by ConditionTarget
    CONDITION_REACTION_TO           = 34,                   // ConditionTarget  rankMask       0                  true if object's reaction matches rankMask object specified by ConditionTarget
    CONDITION_DISTANCE_TO           = 35,                   // ConditionTarget  distance       ComparisonType     true if object and ConditionTarget are within distance given by parameters
    CONDITION_ALIVE                 = 36,                   // 0                0              0                  true if unit is alive
    CONDITION_HP_VAL                = 37,                   // hpVal            ComparisonType 0                  true if unit's hp matches given value
    CONDITION_HP_PCT                = 38,                   // hpPct            ComparisonType 0                  true if unit's hp matches given pct
    CONDITION_AREA_EXPLORED         = 39,                   // AreaID
    CONDITION_SCENE_SEEN            = 40,                   // SceneID
    CONDITION_QUEST_OBJECTIVE_DONE  = 41,                   // QuestID          ObjectiveID    Count   
    CONDITION_SCENE_TRIGER_EVENT    = 42,                   // SceneID
    CONDITION_GARRRISON_BUILDING    = 43,                   // BuildType        Lvl
    CONDITION_SCENARION_STEP        = 44,                   // ScenarioID       stepID
    CONDITION_REALM_ACHIEVEMENT     = 45,                   // achievement_id   0              0                  true if realm achievement is complete
    CONDITION_IN_WATER              = 46,                   // 0                0              0                  true if unit in water
    CONDITION_TERRAIN_SWAP          = 47,                   // terrainSwap      0              0                  true if object is in terrainswap
    CONDITION_STAND_STATE           = 48,                   // stateType        state          0                  true if unit matches specified sitstate (0,x: has exactly state x; 1,0: any standing state; 1,1: any sitting state;)
    CONDITION_CLASS_HALL_ADVANCEMENT= 49,                   // talentID         0              0                  true if learned talent at class hall.
    CONDITION_CURRENCY              = 50,                   // currency_id      countMin       countMax           true if has #countMin and countMax
    CONDITION_CRITERIA              = 51,                   // criteriatree_id  0              0                  true if Criteria is complete
    CONDITION_CRITERIA_TREE         = 52,                   // criteriatree_id  0              0                  true if CriteriaTree is complete
    CONDITION_MODIFIER_TREE         = 53,                   // modifiertree_id  0              0                  true if ModifierTree is complete
    CONDITION_ARTIFACT_LEVEL        = 54,                   // itemEntry        minLevel       maxLevel           true if in minLevel maxLevel
    CONDITION_SPEC_ID               = 55,                   // specId           0              0                  true if in current spec
    CONDITION_ON_TRANSPORT          = 56,                   //                                                    true if on vehicle
    CONDITION_ARTIFACT_POWER        = 57,                   // itemEntry or 0   minPower       maxPower           true if in minLevel maxLevel
    CONDITION_IN_RAID_OR_GROUP      = 58,                   // 0 - not in raid  isRaid         isGroup
    CONDITION_CURRENCY_ON_WEEK      = 59,                   // currency_id      countMin       countMax           true if has #countMin and countMax
    CONDITION_WORLD_QUEST           = 60,                   // QuestID          0              0                  true if world quest active
    CONDITION_HAS_POWER             = 61,                   // PowerType        > this         < this             true if power > or power < if set
    CONDITION_GAMEMASTER            = 62,                   // 0                0              0                  true if player is GameMaster
    CONDITION_HAS_EMOTE_STATE       = 63,                   // 0                0              0                  true if has EmoteState
    CONDITION_IN_COMBAT             = 64,                   // 0                0              0                  true if in combat
    CONDITION_GET_AMOUNT_STACK_AURA = 65,                   // spell_id         stack          0                  true if player (or target) has aura of spell_id with stack amount 
    CONDITION_TIMEWALKING           = 66,                   // 0                0              0                  true if player is in timewalking.
    CONDITION_ACOUNT_QUEST          = 67,                   // quest_id         0              0                  true if quest_id was rewarded on any char account
    CONDITION_DAILY_QUEST_DONE      = 68,                   // quest id         0              0                  true if daily quest has been completed for the day
    CONDITION_CHARMED               = 69,                   // 0                0              0                  true if unit is currently charmed
    CONDITION_PET_TYPE              = 70,                   // mask             0              0                  true if player has a pet of given type(s)
    CONDITION_TAXI                  = 71,                   // 0                0              0                  true if player is on taxi
    CONDITION_QUESTSTATE            = 72,                   // quest_id         state_mask     0                  true if player is in any of the provided quest states for the quest (1 = not taken, 2 = completed, 8 = in progress, 32 = failed, 64 = rewarded)
    CONDITION_DIFFICULTY_ID         = 73,                   // Difficulty       0              0                  true is map has difficulty id
    CONDITION_HAS_GROUP             = 74,                   // 0                0              0                  true if player is in group
    CONDITION_WEEKLY_QUEST_DONE     = 75,                   // quest            0              0                  true if weekly quest has been completed for the week  
    CONDITION_REPUTATION_VALUE      = 76,                   // faction_id       rep_value      0                  true if reputation value more or equal than rep_value 
    
    CONDITION_MAX                   = 77                    // MAX
};

/*! Documentation on implementing a new ConditionSourceType:
    Step 1: Check for the lowest free ID. Look for CONDITION_SOURCE_TYPE_UNUSED_XX in the enum.
            Then define the new source type.

    Step 2: Determine and map the parameters for the new condition type.

    Step 3: Add a case block to ConditionMgr::isSourceTypeValid with the new condition type
            and validate the parameters.

    Step 4: If your condition can be grouped (determined in step 2), add a rule for it in
            ConditionMgr::CanHaveSourceGroupSet, following the example of the existing types.

    Step 5: Define the maximum available condition targets in ConditionMgr::GetMaxAvailableConditionTargets.

    The following steps only apply if your condition can be grouped:

    Step 6: Determine how you are going to store your conditions. You need to add a new storage container
            for it in ConditionMgr class, along with a function like:
            ConditionList GetConditionsForXXXYourNewSourceTypeXXX(parameters...)

            The above function should be placed in upper level (practical) code that actually
            checks the conditions.

    Step 7: Implement loading for your source type in ConditionMgr::LoadConditions.

    Step 8: Implement memory cleaning for your source type in ConditionMgr::Clean.
*/
enum ConditionSourceType
{
    CONDITION_SOURCE_TYPE_NONE                           = 0,
    CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE         = 1,
    CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE       = 2,
    CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE          = 3,
    CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE       = 4,
    CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE             = 5,
    CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE             = 6,
    CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE          = 7,
    CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE    = 8,
    CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE      = 9,
    CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE        = 10,
    CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE         = 11,
    CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE            = 12,
    CONDITION_SOURCE_TYPE_SPELL_IMPLICIT_TARGET          = 13,
    CONDITION_SOURCE_TYPE_GOSSIP_MENU                    = 14,
    CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION             = 15,
    CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE      = 16,
    CONDITION_SOURCE_TYPE_SPELL                          = 17,
    CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT              = 18,
    CONDITION_SOURCE_TYPE_QUEST_ACCEPT                   = 19,
    CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK                = 20,
    CONDITION_SOURCE_TYPE_VEHICLE_SPELL                  = 21,
    CONDITION_SOURCE_TYPE_SMART_EVENT                    = 22,
    CONDITION_SOURCE_TYPE_PHASE_DEFINITION               = 23,
    CONDITION_SOURCE_TYPE_SPELL_PROC                     = 24,
    CONDITION_SOURCE_TYPE_NPC_VENDOR                     = 25,
    CONDITION_SOURCE_TYPE_AREATRIGGER_ACTION             = 26,
    CONDITION_SOURCE_TYPE_BONUS_LOOT_TEMPLATE            = 27,
    CONDITION_SOURCE_TYPE_VIGNETTE                       = 28,
    CONDITION_SOURCE_TYPE_SEAMLESS_TELEPORT              = 29,
    CONDITION_SOURCE_TYPE_LOOT_ITEM                      = 30,
    CONDITION_SOURCE_TYPE_WORLD_LOOT_TEMPLATE            = 31,
    CONDITION_SOURCE_TYPE_PLAYER_CHOICE                  = 32,
    CONDITION_SOURCE_TYPE_PLAYER_CHOICE_RESPONS          = 33,
    CONDITION_SOURCE_TYPE_WORLD_STATE                    = 34,
    CONDITION_SOURCE_TYPE_MAX                            = 35  //MAX
};

enum ComparisionType
{
    COMP_TYPE_EQ = 0,
    COMP_TYPE_HIGH,
    COMP_TYPE_LOW,
    COMP_TYPE_HIGH_EQ,
    COMP_TYPE_LOW_EQ,
    COMP_TYPE_MAX
};

enum RelationType
{
    RELATION_SELF = 0,
    RELATION_IN_PARTY,
    RELATION_IN_RAID_OR_PARTY,
    RELATION_OWNED_BY,
    RELATION_PASSENGER_OF,
    RELATION_CREATED_BY,
    RELATION_MAX
};

enum InstanceInfo
{
    INSTANCE_INFO_DATA = 0,
    INSTANCE_INFO_GUID_DATA,
    INSTANCE_INFO_BOSS_STATE,
    INSTANCE_INFO_SCENARION_STEP,
};

enum
{
    MAX_CONDITION_TARGETS = 3
};

struct ConditionSourceInfo
{
    ConditionSourceInfo(WorldObject* target0, WorldObject* target1 = nullptr, WorldObject* target2 = nullptr);

    WorldObject* mConditionTargets[MAX_CONDITION_TARGETS]; // an array of targets available for conditions
    Condition* mLastFailedCondition;
};

struct Condition
{
    ConditionSourceType     SourceType;        //SourceTypeOrReferenceId
    uint32                  SourceGroup;
    int32                   SourceEntry;
    uint32                  SourceId;          // So far, only used in CONDITION_SOURCE_TYPE_SMART_EVENT
    uint32                  ElseGroup;
    ConditionTypes          ConditionType;     //ConditionTypeOrReference
    uint32                   ConditionValue1;
    uint32                   ConditionValue2;
    uint32                   ConditionValue3;
    uint32                  ErrorTextId;
    uint32                  ReferenceId;
    uint32                  ScriptId;
    uint8                   ConditionTarget;
    bool                    NegativeCondition;

    Condition();

    bool Meets(ConditionSourceInfo& sourceInfo);
    uint32 GetSearcherTypeMaskForCondition();
    bool isLoaded() const;
    uint32 GetMaxAvailableConditionTargets();
};

typedef std::list<Condition*> ConditionList;
typedef std::map<uint32, ConditionList> ConditionTypeContainer;
typedef std::map<ConditionSourceType, ConditionTypeContainer> ConditionContainer;
typedef std::map<uint32, ConditionTypeContainer> CreatureSpellConditionContainer;
typedef std::map<uint32, ConditionTypeContainer> NpcVendorConditionContainer;
typedef std::map<std::pair<int32, uint32 /*SAI source_type*/>, ConditionTypeContainer> SmartEventConditionContainer;
typedef std::map<int32 /*zoneId*/, ConditionTypeContainer> PhaseDefinitionConditionContainer;
typedef std::map<uint32 /*areatrigger id*/, ConditionTypeContainer> AreaTriggerConditionContainer;
typedef std::map<uint32 /*itemId*/, ConditionTypeContainer> ItemLootConditionContainer;

typedef std::map<uint32, ConditionList> ConditionReferenceContainer;//only used for references

class ConditionMgr
{
        ConditionMgr();
        ~ConditionMgr();

    public:

        static ConditionMgr* instance();

        void LoadConditions(bool isReload = false);
        bool isConditionTypeValid(Condition* cond);
        ConditionList GetConditionReferences(uint32 refId);

        uint32 GetSearcherTypeMaskForConditionList(ConditionList const& conditions);
        bool IsObjectMeetToConditions(WorldObject* object, ConditionList const& conditions);
        bool IsObjectMeetToConditions(WorldObject* object1, WorldObject* object2, ConditionList const& conditions);
        bool IsObjectMeetToConditions(ConditionSourceInfo& sourceInfo, ConditionList const& conditions);
        bool CanHaveSourceGroupSet(ConditionSourceType sourceType) const;
        bool CanHaveSourceIdSet(ConditionSourceType sourceType) const;
        ConditionList GetConditionsForNotGroupedEntry(ConditionSourceType sourceType, uint32 entry);
        ConditionList GetConditionsForSpellClickEvent(uint32 creatureId, uint32 spellId);
        ConditionList GetConditionsForSmartEvent(int64 entryOrGuid, uint32 eventId, uint32 sourceType);
        ConditionList GetConditionsForVehicleSpell(uint32 creatureId, uint32 spellId);
        ConditionList GetConditionsForNpcVendorEvent(uint32 creatureId, uint32 itemId);
        ConditionList GetConditionsForPhaseDefinition(uint32 zone, uint32 entry);
        ConditionList GetConditionsForAreaTriggerAction(uint32 areaTriggerId, uint32 actionId);
        ConditionList GetConditionsForItemLoot(uint32 creatureId, uint32 itemId);
		bool IsObjectMeetingSmartEventConditions(int64 entryOrGuid, uint32 eventId, uint32 sourceType, Unit* unit, WorldObject* baseObject) const;
        
        static bool IsPlayerMeetingCondition(Unit* unit, int32 conditionID, bool send = false);
        static bool IsPlayerMeetingCondition(Unit* unit, PlayerConditionEntry const* condition);

    private:
        bool isSourceTypeValid(Condition* cond);
        bool addToLootTemplate(Condition* cond, LootTemplate* loot);
        bool addToGossipMenus(Condition* cond);
        bool addToGossipMenuItems(Condition* cond);
        bool addToSpellImplicitTargetConditions(Condition* cond);
        bool IsObjectMeetToConditionList(ConditionSourceInfo& sourceInfo, ConditionList const& conditions);

        void Clean(); // free up resources
        std::list<Condition*> AllocatedMemoryStore; // some garbage collection :)

        ConditionContainer                ConditionStore;
        ConditionReferenceContainer       ConditionReferenceStore;
        CreatureSpellConditionContainer   VehicleSpellConditionStore;
        CreatureSpellConditionContainer   SpellClickEventConditionStore;
        NpcVendorConditionContainer       NpcVendorConditionContainerStore;
        SmartEventConditionContainer      SmartEventConditionStore;
        PhaseDefinitionConditionContainer PhaseDefinitionsConditionStore;
        AreaTriggerConditionContainer     AreaTriggerConditionStore;
        ItemLootConditionContainer        ItemLootConditionStore;
};

template <class T>
bool CompareValues(ComparisionType type, T val1, T val2);

#define sConditionMgr ConditionMgr::instance()

#endif
