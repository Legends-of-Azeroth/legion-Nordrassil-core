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

#ifndef _ACCMGR_H
#define _ACCMGR_H

#include <string>
#include "Define.h"

enum class AccountOpResult : uint8
{
    AOR_OK,
    AOR_NAME_TOO_LONG,
    AOR_PASS_TOO_LONG,
    AOR_EMAIL_TOO_LONG,
    AOR_NAME_ALREADY_EXIST,
    AOR_NAME_NOT_EXIST,
    AOR_DB_INTERNAL_ERROR,
	AOR_ACCOUNT_BAD_LINK
};

#define MAX_PASS_STR 32
#define MAX_ACCOUNT_STR 32
#define MAX_EMAIL_STR 64

namespace AccountMgr
{
    AccountOpResult CreateAccount(std::string username, std::string password, uint32 bnetAccountId = 0, uint8 bnetIndex = 0);
    AccountOpResult DeleteAccount(uint32 accountId);
    AccountOpResult ChangeUsername(uint32 accountId, std::string newUsername, std::string newPassword);
    AccountOpResult ChangePassword(uint32 accountId, std::string newPassword);
    bool CheckPassword(uint32 accountId, std::string password);

    uint32 GetId(std::string username);
    uint32 GetSecurity(uint32 accountId);
    uint32 GetSecurity(uint32 accountId, int32 realmId);
    bool GetName(uint32 accountId, std::string& name);
    uint32 GetCharactersCount(uint32 accountId);
    std::string CalculateShaPassHash(std::string& name, std::string& password);

    bool IsPlayerAccount(uint32 gmlevel);
    bool IsModeratorAccount(uint32 gmlevel);
    bool IsGMAccount(uint32 gmlevel);
    bool IsAdminAccount(uint32 gmlevel);
    bool IsConsoleAccount(uint32 gmlevel);
};

#endif
