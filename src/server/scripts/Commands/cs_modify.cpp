/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include <stdlib.h>

class modify_commandscript : public CommandScript
{
public:
    modify_commandscript() : CommandScript("modify_commandscript") { }

    ChatCommand* GetCommands() const override
    {
        static ChatCommand modifyspeedCommandTable[] =
        {
            { "fly",            SEC_MODERATOR,      false, &HandleModifyFlyCommand,           "", NULL },
            { "all",            SEC_MODERATOR,      false, &HandleModifyASpeedCommand,        "", NULL },
            { "walk",           SEC_MODERATOR,      false, &HandleModifySpeedCommand,         "", NULL },
            { "backwalk",       SEC_MODERATOR,      false, &HandleModifyBWalkCommand,         "", NULL },
            { "swim",           SEC_MODERATOR,      false, &HandleModifySwimCommand,          "", NULL },
            { "",               SEC_MODERATOR,      false, &HandleModifyASpeedCommand,        "", NULL },
            { NULL,             0,                  false, NULL,                              "", NULL }
        };
        static ChatCommand modifyCommandTable[] =
        {
            { "hp",             SEC_MODERATOR,      false, &HandleModifyHPCommand,            "", NULL },
            { "mana",           SEC_MODERATOR,      false, &HandleModifyManaCommand,          "", NULL },
            { "rage",           SEC_MODERATOR,      false, &HandleModifyRageCommand,          "", NULL },
            { "runicpower",     SEC_MODERATOR,      false, &HandleModifyRunicPowerCommand,    "", NULL },
            { "energy",         SEC_MODERATOR,      false, &HandleModifyEnergyCommand,        "", NULL },
            { "money",          SEC_MODERATOR,      false, &HandleModifyMoneyCommand,         "", NULL },
            { "scale",          SEC_MODERATOR,      false, &HandleModifyScaleCommand,         "", NULL },
            { "bit",            SEC_MODERATOR,      false, &HandleModifyBitCommand,           "", NULL },
            { "faction",        SEC_MODERATOR,      false, &HandleModifyFactionCommand,       "", NULL },
            { "spell",          SEC_MODERATOR,      false, &HandleModifySpellCommand,         "", NULL },
            { "talentpoints",   SEC_MODERATOR,      false, &HandleModifyTalentCommand,        "", NULL },
            { "mount",          SEC_MODERATOR,      false, &HandleModifyMountCommand,         "", NULL },
            { "honor",          SEC_MODERATOR,      false, &HandleModifyHonorCommand,         "", NULL },
            { "reputation",     SEC_GAMEMASTER,     false, &HandleModifyRepCommand,           "", NULL },
            { "drunk",          SEC_MODERATOR,      false, &HandleModifyDrunkCommand,         "", NULL },
            { "standstate",     SEC_GAMEMASTER,     false, &HandleModifyStandStateCommand,    "", NULL },
            { "phase",          SEC_ADMINISTRATOR,  false, &HandleModifyPhaseCommand,         "", NULL },
            { "phaseids",       SEC_ADMINISTRATOR,  false, &HandleModifyPhaseIDsCommand, "", NULL },
            { "gender",         SEC_GAMEMASTER,     false, &HandleModifyGenderCommand,        "", NULL },
            { "power",          SEC_GAMEMASTER,     false, &HandleModifyPowerCommand,         "", NULL },
            { "currency",       SEC_GAMEMASTER,     false, &HandleModifyCurrencyCommand,      "", NULL },
            { "speed",          SEC_MODERATOR,      false, NULL,           "", modifyspeedCommandTable },
            { "artifactxp",     SEC_GAMEMASTER,     false, &HandleModifyArtifactXPCommand,    "", NULL },
            { "ilvl",           SEC_ADMINISTRATOR,  false, &HandleModifyItemsLevel,           "", NULL },
            { NULL,             0,                  false, NULL,                              "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "morph",          SEC_GAMEMASTER,     false, &HandleModifyMorphCommand,          "", NULL },
            { "demorph",        SEC_GAMEMASTER,     false, &HandleDeMorphCommand,              "", NULL },
            { "modify",         SEC_MODERATOR,      false, NULL,                 "", modifyCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    //Edit Player HP
    static bool HandleModifyHPCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int32 hp = atoi((char*)args);
        int32 hpm = atoi((char*)args);

        if (hp < 1 || hpm < 1 || hpm < hp)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_HP, handler->GetNameLink(target).c_str(), hp, hpm);
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_HP_CHANGED, handler->GetNameLink().c_str(), hp, hpm);

        target->SetMaxHealth(hpm);
        target->SetHealth(hp);

        return true;
    }

    //Edit Player Mana
    static bool HandleModifyManaCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int32 mana = atoi((char*)args);
        int32 manam = atoi((char*)args);

        if (mana <= 0 || manam <= 0 || manam < mana)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_MANA, handler->GetNameLink(target).c_str(), mana, manam);
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_MANA_CHANGED, handler->GetNameLink().c_str(), mana, manam);

        target->SetMaxPower(POWER_MANA, manam);
        target->SetPower(POWER_MANA, mana);

        return true;
    }

    //Edit Player Energy
    static bool HandleModifyEnergyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // char* pmana = strtok((char*)args, " ");
        // if (!pmana)
        //     return false;

        // char* pmanaMax = strtok(NULL, " ");
        // if (!pmanaMax)
        //     return false;

        // int32 manam = atoi(pmanaMax);
        // int32 mana = atoi(pmana);

        int32 energy = atoi((char*)args)*10;
        int32 energym = atoi((char*)args)*10;

        if (energy <= 0 || energym <= 0 || energym < energy)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_ENERGY, handler->GetNameLink(target).c_str(), energy/10, energym/10);
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_ENERGY_CHANGED, handler->GetNameLink().c_str(), energy/10, energym/10);

        target->SetMaxPower(POWER_ENERGY, energym);
        target->SetPower(POWER_ENERGY, energy);

        TC_LOG_DEBUG(LOG_FILTER_GENERAL, handler->GetTrinityString(LANG_CURRENT_ENERGY), target->GetMaxPower(POWER_ENERGY));

        return true;
    }

    //Edit Player Rage
    static bool HandleModifyRageCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // char* pmana = strtok((char*)args, " ");
        // if (!pmana)
        //     return false;

        // char* pmanaMax = strtok(NULL, " ");
        // if (!pmanaMax)
        //     return false;

        // int32 manam = atoi(pmanaMax);
        // int32 mana = atoi(pmana);

        int32 rage = atoi((char*)args)*10;
        int32 ragem = atoi((char*)args)*10;

        if (rage <= 0 || ragem <= 0 || ragem < rage)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_RAGE, handler->GetNameLink(target).c_str(), rage/10, ragem/10);
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_RAGE_CHANGED, handler->GetNameLink().c_str(), rage/10, ragem/10);

        target->SetMaxPower(POWER_RAGE, ragem);
        target->SetPower(POWER_RAGE, rage);

        return true;
    }

    // Edit Player Runic Power
    static bool HandleModifyRunicPowerCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int32 rune = atoi((char*)args)*10;
        int32 runem = atoi((char*)args)*10;

        if (rune <= 0 || runem <= 0 || runem < rune)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_RUNIC_POWER, handler->GetNameLink(target).c_str(), rune/10, runem/10);
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_RUNIC_POWER_CHANGED, handler->GetNameLink().c_str(), rune/10, runem/10);

        target->SetMaxPower(POWER_RUNIC_POWER, runem);
        target->SetPower(POWER_RUNIC_POWER, rune);

        return true;
    }

    //Edit Player Faction
    static bool HandleModifyFactionCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* pfactionid = handler->extractKeyFromLink((char*)args, "Hfaction");

        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!pfactionid)
        {
            if (target)
            {
                uint32 factionid = target->getFaction();
                uint32 flag      = target->GetUInt32Value(UNIT_FIELD_FLAGS);
                uint32 npcflag   = target->GetUInt32Value(UNIT_FIELD_NPC_FLAGS);
                uint32 dyflag    = target->GetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS);
                handler->PSendSysMessage(LANG_CURRENT_FACTION, target->GetGUID().GetGUIDLow(), factionid, flag, npcflag, dyflag);
            }
            return true;
        }

        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 factionid = atoi(pfactionid);
        uint32 flag;

        char *pflag = strtok(NULL, " ");
        if (!pflag)
            flag = target->GetUInt32Value(UNIT_FIELD_FLAGS);
        else
            flag = atoi(pflag);

        char* pnpcflag = strtok(NULL, " ");

        uint32 npcflag;
        if (!pnpcflag)
            npcflag   = target->GetUInt32Value(UNIT_FIELD_NPC_FLAGS);
        else
            npcflag = atoi(pnpcflag);

        char* pdyflag = strtok(NULL, " ");

        uint32  dyflag;
        if (!pdyflag)
            dyflag   = target->GetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS);
        else
            dyflag = atoi(pdyflag);

        if (!sFactionTemplateStore.LookupEntry(factionid))
        {
            handler->PSendSysMessage(LANG_WRONG_FACTION, factionid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_FACTION, target->GetGUID().GetGUIDLow(), factionid, flag, npcflag, dyflag);

        target->setFaction(factionid);
        target->SetUInt32Value(UNIT_FIELD_FLAGS, flag);
        target->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, npcflag);
        target->SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, dyflag);

        return true;
    }

    //Edit Player Spell
    static bool HandleModifySpellCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* pspellflatid = strtok((char*)args, " ");
        if (!pspellflatid)
            return false;

        char* pop = strtok(NULL, " ");
        if (!pop)
            return false;

        char* pval = strtok(NULL, " ");
        if (!pval)
            return false;

        uint16 mark;

        char* pmark = strtok(NULL, " ");

        uint8 spellflatid = atoi(pspellflatid);
        uint8 op   = atoi(pop);
        uint16 val = atoi(pval);
        if (!pmark)
            mark = 65535;
        else
            mark = atoi(pmark);

        Player* target = handler->getSelectedPlayer();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_SPELLFLATID, spellflatid, val, mark, handler->GetNameLink(target).c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_SPELLFLATID_CHANGED, handler->GetNameLink().c_str(), spellflatid, val, mark);

        /*WorldPacket data(SMSG_SET_FLAT_SPELL_MODIFIER, (1+1+2+2));
        data << uint8(spellflatid);
        data << uint8(op);
        data << uint16(val);
        data << uint16(mark);
        target->SendDirectMessage(&data);*/

        return true;
    }

    //Edit Player TP
    static bool HandleModifyTalentCommand (ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int tp = atoi((char*)args);
        if (tp < 0)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            // check online security
            if (handler->HasLowerSecurity(target->ToPlayer(), ObjectGuid::Empty))
                return false;

            target->ToPlayer()->SendTalentsInfoData(false);
            return true;
        }
        if (target->ToCreature()->isPet())
        {
            Unit* owner = target->GetOwner();
            if (owner && owner->GetTypeId() == TYPEID_PLAYER && ((Pet*)target)->IsPermanentPetFor(owner->ToPlayer()))
            {
                // check online security
                if (handler->HasLowerSecurity(owner->ToPlayer(), ObjectGuid::Empty))
                    return false;
                owner->ToPlayer()->SendTalentsInfoData(true);
                return true;
            }
        }
        handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
        handler->SetSentErrorMessage(true);
        return false;
    }

    //Edit Player Aspeed
    static bool HandleModifyASpeedCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float ASpeed = (float)atof((char*)args);

        if (ASpeed > 50.0f || ASpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        std::string targetNameLink = handler->GetNameLink(target);

        if (target->isInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, targetNameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_ASPEED, ASpeed, targetNameLink.c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_ASPEED_CHANGED, handler->GetNameLink().c_str(), ASpeed);

        target->SetSpeed(MOVE_WALK,    ASpeed, true);
        target->SetSpeed(MOVE_RUN,     ASpeed, true);
        target->SetSpeed(MOVE_SWIM,    ASpeed, true);
        //target->SetSpeed(MOVE_TURN,    ASpeed, true);
        target->SetSpeed(MOVE_FLIGHT,     ASpeed, true);
        return true;
    }

    //Edit Player Speed
    static bool HandleModifySpeedCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float Speed = (float)atof((char*)args);

        if (Speed > 50.0f || Speed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        std::string targetNameLink = handler->GetNameLink(target);

        if (target->isInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, targetNameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_SPEED, Speed, targetNameLink.c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_SPEED_CHANGED, handler->GetNameLink().c_str(), Speed);

        target->SetSpeed(MOVE_RUN, Speed, true);

        return true;
    }

    //Edit Player Swim Speed
    static bool HandleModifySwimCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float Swim = (float)atof((char*)args);

        if (Swim > 50.0f || Swim < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        std::string targetNameLink = handler->GetNameLink(target);

        if (target->isInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, targetNameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_SWIM_SPEED, Swim, targetNameLink.c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_SWIM_SPEED_CHANGED, handler->GetNameLink().c_str(), Swim);

        target->SetSpeed(MOVE_SWIM, Swim, true);

        return true;
    }

    //Edit Player Walk Speed
    static bool HandleModifyBWalkCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float BSpeed = (float)atof((char*)args);

        if (BSpeed > 50.0f || BSpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        std::string targetNameLink = handler->GetNameLink(target);

        if (target->isInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, targetNameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_BACK_SPEED, BSpeed, targetNameLink.c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_BACK_SPEED_CHANGED, handler->GetNameLink().c_str(), BSpeed);

        target->SetSpeed(MOVE_RUN_BACK, BSpeed, true);

        return true;
    }

    //Edit Player Fly
    static bool HandleModifyFlyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float FSpeed = (float)atof((char*)args);

        if (FSpeed > 50.0f || FSpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_CHANGE_FLY_SPEED, FSpeed, handler->GetNameLink(target).c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOURS_FLY_SPEED_CHANGED, handler->GetNameLink().c_str(), FSpeed);

        target->SetSpeed(MOVE_FLIGHT, FSpeed, true);

        return true;
    }

    //Edit Player or Creature Scale
    static bool HandleModifyScaleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        float Scale = (float)atof((char*)args);
        if (Scale > 10.0f || Scale < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (Player* player = target->ToPlayer())
        {
            // check online security
            if (handler->HasLowerSecurity(player, ObjectGuid::Empty))
                return false;

            handler->PSendSysMessage(LANG_YOU_CHANGE_SIZE, Scale, handler->GetNameLink(player).c_str());
            if (handler->needReportToTarget(player))
                (ChatHandler(player)).PSendSysMessage(LANG_YOURS_SIZE_CHANGED, handler->GetNameLink().c_str(), Scale);
        }

        target->SetObjectScale(Scale);

        return true;
    }

    //Enable Player mount
    static bool HandleModifyMountCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint16 mId = 1147;
        float speed = (float)15;
        uint32 num = 0;

        num = atoi((char*)args);
        switch (num)
        {
        case 1:
            mId=14340;
            break;
        case 2:
            mId=4806;
            break;
        case 3:
            mId=6471;
            break;
        case 4:
            mId=12345;
            break;
        case 5:
            mId=6472;
            break;
        case 6:
            mId=6473;
            break;
        case 7:
            mId=10670;
            break;
        case 8:
            mId=10719;
            break;
        case 9:
            mId=10671;
            break;
        case 10:
            mId=10672;
            break;
        case 11:
            mId=10720;
            break;
        case 12:
            mId=14349;
            break;
        case 13:
            mId=11641;
            break;
        case 14:
            mId=12244;
            break;
        case 15:
            mId=12242;
            break;
        case 16:
            mId=14578;
            break;
        case 17:
            mId=14579;
            break;
        case 18:
            mId=14349;
            break;
        case 19:
            mId=12245;
            break;
        case 20:
            mId=14335;
            break;
        case 21:
            mId=207;
            break;
        case 22:
            mId=2328;
            break;
        case 23:
            mId=2327;
            break;
        case 24:
            mId=2326;
            break;
        case 25:
            mId=14573;
            break;
        case 26:
            mId=14574;
            break;
        case 27:
            mId=14575;
            break;
        case 28:
            mId=604;
            break;
        case 29:
            mId=1166;
            break;
        case 30:
            mId=2402;
            break;
        case 31:
            mId=2410;
            break;
        case 32:
            mId=2409;
            break;
        case 33:
            mId=2408;
            break;
        case 34:
            mId=2405;
            break;
        case 35:
            mId=14337;
            break;
        case 36:
            mId=6569;
            break;
        case 37:
            mId=10661;
            break;
        case 38:
            mId=10666;
            break;
        case 39:
            mId=9473;
            break;
        case 40:
            mId=9476;
            break;
        case 41:
            mId=9474;
            break;
        case 42:
            mId=14374;
            break;
        case 43:
            mId=14376;
            break;
        case 44:
            mId=14377;
            break;
        case 45:
            mId=2404;
            break;
        case 46:
            mId=2784;
            break;
        case 47:
            mId=2787;
            break;
        case 48:
            mId=2785;
            break;
        case 49:
            mId=2736;
            break;
        case 50:
            mId=2786;
            break;
        case 51:
            mId=14347;
            break;
        case 52:
            mId=14346;
            break;
        case 53:
            mId=14576;
            break;
        case 54:
            mId=9695;
            break;
        case 55:
            mId=9991;
            break;
        case 56:
            mId=6448;
            break;
        case 57:
            mId=6444;
            break;
        case 58:
            mId=6080;
            break;
        case 59:
            mId=6447;
            break;
        case 60:
            mId=4805;
            break;
        case 61:
            mId=9714;
            break;
        case 62:
            mId=6448;
            break;
        case 63:
            mId=6442;
            break;
        case 64:
            mId=14632;
            break;
        case 65:
            mId=14332;
            break;
        case 66:
            mId=14331;
            break;
        case 67:
            mId=8469;
            break;
        case 68:
            mId=2830;
            break;
        case 69:
            mId=2346;
            break;
        default:
            handler->SendSysMessage(LANG_NO_MOUNT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        handler->PSendSysMessage(LANG_YOU_GIVE_MOUNT, handler->GetNameLink(target).c_str());
        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_MOUNT_GIVED, handler->GetNameLink().c_str());

        target->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP);
        target->Mount(mId);
        target->SetSpeed(MOVE_RUN, speed);
        target->SetSpeed(MOVE_SWIM, speed);

        return true;
    }

    //Edit Player money
    static bool HandleModifyMoneyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        int64 addmoney = atoi((char*)args);

        uint64 moneyuser = target->GetMoney();

        if (addmoney < 0)
        {
            int64 newmoney = int64(moneyuser) + addmoney;

            TC_LOG_DEBUG(LOG_FILTER_GENERAL, handler->GetTrinityString(LANG_CURRENT_MONEY), moneyuser, addmoney, newmoney);
            if (newmoney <= 0)
            {
                handler->PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, handler->GetNameLink(target).c_str());
                if (handler->needReportToTarget(target))
                    (ChatHandler(target)).PSendSysMessage(LANG_YOURS_ALL_MONEY_GONE, handler->GetNameLink().c_str());

                target->SetMoney(0);
            }
            else
            {
                if (newmoney > MAX_MONEY_AMOUNT)
                    newmoney = MAX_MONEY_AMOUNT;

                handler->PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), handler->GetNameLink(target).c_str());
                if (handler->needReportToTarget(target))
                    (ChatHandler(target)).PSendSysMessage(LANG_YOURS_MONEY_TAKEN, handler->GetNameLink().c_str(), abs(addmoney));
                target->SetMoney(newmoney);
            }
        }
        else
        {
            handler->PSendSysMessage(LANG_YOU_GIVE_MONEY, uint32(addmoney), handler->GetNameLink(target).c_str());
            if (handler->needReportToTarget(target))
                (ChatHandler(target)).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, handler->GetNameLink().c_str(), uint32(addmoney));

            if (addmoney >=MAX_MONEY_AMOUNT)
                target->SetMoney(MAX_MONEY_AMOUNT);
            else
                target->ModifyMoney(int64(addmoney));
        }

        TC_LOG_DEBUG(LOG_FILTER_GENERAL, handler->GetTrinityString(LANG_NEW_MONEY), moneyuser, uint32(addmoney), target->GetMoney());
        return true;
    }

    //Edit Unit field
    static bool HandleModifyBitCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity(target->ToPlayer(), ObjectGuid::Empty))
            return false;

        char* pField = strtok((char*)args, " ");
        if (!pField)
            return false;

        char* pBit = strtok(NULL, " ");
        if (!pBit)
            return false;

        uint16 field = atoi(pField);
        uint32 bit   = atoi(pBit);

        if (field < OBJECT_END || field >= target->GetValuesCount())
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (bit < 1 || bit > 32)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->HasFlag(field, (1<<(bit-1))))
        {
            target->RemoveFlag(field, (1<<(bit-1)));
            handler->PSendSysMessage(LANG_REMOVE_BIT, bit, field);
        }
        else
        {
            target->SetFlag(field, (1<<(bit-1)));
            handler->PSendSysMessage(LANG_SET_BIT, bit, field);
        }
        return true;
    }

    static bool HandleModifyHonorCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        int32 amount = (uint32)atoi(args);

        target->RewardHonor(NULL, 0, amount, false);

        handler->PSendSysMessage(LANG_COMMAND_MODIFY_HONOR, handler->GetNameLink(target).c_str(), target->GetCurrency(CURRENCY_TYPE_HONOR_POINTS));

        return true;
    }

    static bool HandleModifyDrunkCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint8 drunklevel = (uint8)atoi(args);
        if (drunklevel > 100)
            drunklevel = 100;

        if (Player* target = handler->getSelectedPlayer())
            target->SetDrunkValue(drunklevel);

        return true;
    }

    static bool HandleModifyRepCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        char* factionTxt = handler->extractKeyFromLink((char*)args, "Hfaction");
        if (!factionTxt)
            return false;

        uint32 factionId = atoi(factionTxt);

        int32 amount = 0;
        char *rankTxt = strtok(NULL, " ");
        if (!factionTxt || !rankTxt)
            return false;

        amount = atoi(rankTxt);
        if ((amount == 0) && (rankTxt[0] != '-') && !isdigit(rankTxt[0]))
        {
            handler->PSendSysMessage(LANG_COMMAND_FACTION_INVPARAM, rankTxt);
            handler->SetSentErrorMessage(true);
            return false;
        }

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (!factionEntry)
        {
            handler->PSendSysMessage(LANG_COMMAND_FACTION_UNKNOWN, factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (factionEntry->ReputationIndex < 0)
        {
            handler->PSendSysMessage(LANG_COMMAND_FACTION_NOREP_ERROR, factionEntry->Name->Str[LOCALE_enUS], factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sDB2Manager.GetFactionParagon(factionEntry->ParagonFactionID))
            target->GetReputationMgr().ModifyParagonReputation(factionEntry, amount);
        else
            target->GetReputationMgr().ModifyReputation(factionEntry, amount);
        handler->PSendSysMessage(LANG_COMMAND_MODIFY_REP, factionEntry->Name->Str[LOCALE_enUS], factionId,
            handler->GetNameLink(target).c_str(), target->GetReputationMgr().GetReputation(factionEntry));
        return true;
    }

    //morph creature or player
    static bool HandleModifyMorphCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 display_id = (uint32)atoi((char*)args);

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        // check online security
        else if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity(target->ToPlayer(), ObjectGuid::Empty))
            return false;

        target->SetDisplayId(display_id);
        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
        // if (Creature* crea = target->ToCreature())
            // crea->SetOutfit(display_id);

        return true;
    }

    //set temporary phase mask for player
    static bool HandleModifyPhaseCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 phasemask = (uint32)atoi((char*)args);

        Unit* target = handler->getSelectedUnit();    
        if (target)    
        {    
            if (target->GetTypeId() == TYPEID_PLAYER)    
                target->ToPlayer()->GetPhaseMgr().SetCustomPhase(phasemask);    
            else    
                target->SetPhaseMask(phasemask, true);    
        }    
        else    
            handler->GetSession()->GetPlayer()->GetPhaseMgr().SetCustomPhase(phasemask);

        return true;
    }

    
    static bool HandleModifyPhaseIDsCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::set<uint32> phaseIds;
        Tokenizer phasesToken((char*)args, ' ', 100);
        for (Tokenizer::const_iterator itr = phasesToken.begin(); itr != phasesToken.end(); ++itr)
        {
            if (PhaseEntry const* phase = sPhaseStore.LookupEntry(uint32(strtoull(*itr, nullptr, 10))))
                phaseIds.insert(phase->ID);
        }

        Unit* target = handler->getSelectedUnit();
        if (target)
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->ToPlayer()->SetPhaseId(phaseIds, true);
            else
                return false;
        }
        else
            handler->GetSession()->GetPlayer()->SetPhaseId(phaseIds, true);

        return true;
    }
    //change standstate
    static bool HandleModifyStandStateCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 anim_id = atoi((char*)args);
        handler->GetSession()->GetPlayer()->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, anim_id);

        return true;
    }

    static bool HandleModifyGenderCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        PlayerInfo const* info = sObjectMgr->GetPlayerInfo(target->getRace(), target->getClass());
        if (!info)
            return false;

        char const* gender_str = (char*)args;
        int gender_len = strlen(gender_str);

        Gender gender;

        if (!strncmp(gender_str, "male", gender_len))            // MALE
        {
            if (target->getGender() == GENDER_MALE)
                return true;

            gender = GENDER_MALE;
        }
        else if (!strncmp(gender_str, "female", gender_len))    // FEMALE
        {
            if (target->getGender() == GENDER_FEMALE)
                return true;

            gender = GENDER_FEMALE;
        }
        else
        {
            handler->SendSysMessage(LANG_MUST_MALE_OR_FEMALE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Set gender
        target->SetByteValue(UNIT_FIELD_BYTES_0, 2, gender);
        target->SetByteValue(PLAYER_FIELD_BYTES_3, 0, gender);

        // Change display ID
        target->InitDisplayIds();

        char const* gender_full = gender ? "female" : "male";

        handler->PSendSysMessage(LANG_YOU_CHANGE_GENDER, handler->GetNameLink(target).c_str(), gender_full);

        if (handler->needReportToTarget(target))
            (ChatHandler(target)).PSendSysMessage(LANG_YOUR_GENDER_CHANGED, gender_full, handler->GetNameLink().c_str());

        return true;
    }

    static bool HandleModifyPowerCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* power_str = strtok((char*)args, " ");
        char* value_str = strtok(NULL, " ");

        if (!power_str || !value_str)
            return false;

        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 power = (uint32)atoi(power_str);

        if (power >= MAX_POWERS)
            return false;

        uint32 value = (uint32)atoi(value_str);

        target->SetPower(Powers(power), value);

        return true;
    }
//demorph player or unit
    static bool HandleDeMorphCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        // check online security
        else if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity(target->ToPlayer(), ObjectGuid::Empty))
            return false;

        target->DeMorph();

        return true;
    }

    static bool HandleModifyCurrencyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* scurrencyId = strtok((char*)args, " ");
        if (!scurrencyId)
            return false;

        CurrencyTypesEntry const* currencyType = sCurrencyTypesStore.LookupEntry(atoi(scurrencyId));
        if (!currencyType)
            return false;

        char* samount = strtok(nullptr, " ");
        if (!samount)
            return false;

        int32 amount = atoi(samount);
        if (!amount)
            return false;

        target->ModifyCurrency(currencyType->ID, amount * sDB2Manager.GetCurrencyPrecision(currencyType->ID), true, true, false, false);

        return true;
    }

    static bool HandleModifyArtifactXPCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* samount = strtok((char*)args, " ");
        if (!samount)
            return false;

        uint64 amount = atoll(samount);
        if (!amount)
            return false;

        if (Item* artifact = target->GetArtifactWeapon())
        {
            artifact->SetUInt64Value(ITEM_FIELD_ARTIFACT_XP, artifact->GetUInt64Value(ITEM_FIELD_ARTIFACT_XP) + amount);
            artifact->SetState(ITEM_CHANGED, target);
        }
        return true;
    }

    static bool HandleModifyItemsLevel(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* samount = strtok((char*) args, " ");
        if (!samount)
            return false;

        float amount = atof(samount);
        if (!amount)
            return false;

        if (amount < 0.f || amount > 1000.f)
            return false;

        target->SetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::EquippedAvgItemLevel, amount);
        if (target->HasPvpStatsScalingEnabled())
        {
            target->SendUpdateStat(USM_ALL);
            target->SendUpdateCR(CR_PVP_UPDATE_MASK);
        }
        return true;
    }
};

void AddSC_modify_commandscript()
{
    new modify_commandscript();
}
