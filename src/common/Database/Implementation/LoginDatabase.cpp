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

#include "LoginDatabase.h"

void LoginDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_LOGINDATABASE_STATEMENTS);

    PrepareStatement(LOGIN_SEL_REALMLIST, "SELECT id, name, address, localAddress, localSubnetMask, port, icon, flag, timezone, allowedSecurityLevel, population, gamebuild, Region, Battlegroup FROM realmlist WHERE flag <> 3 ORDER BY name", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_DEL_EXPIRED_IP_BANS, "DELETE FROM ip_banned WHERE unbandate<>bandate AND unbandate<=UNIX_TIMESTAMP()", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_EXPIRED_ACCOUNT_BANS, "UPDATE account_banned SET active = 0 WHERE active = 1 AND unbandate<>bandate AND unbandate<=UNIX_TIMESTAMP()", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_IP_INFO, "SELECT * FROM ip_banned WHERE ip = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_IP_BANNED_ALL, "SELECT ip, bandate, unbandate, bannedby, banreason FROM ip_banned WHERE (bandate = unbandate OR unbandate > UNIX_TIMESTAMP()) ORDER BY unbandate", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_IP_BANNED_BY_IP, "SELECT ip, bandate, unbandate, bannedby, banreason FROM ip_banned WHERE (bandate = unbandate OR unbandate > UNIX_TIMESTAMP()) AND ip LIKE CONCAT('%%', ?, '%%') ORDER BY unbandate", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_BANNED, "SELECT bandate, unbandate FROM account_banned WHERE id = ? AND active = 1", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_BANNED_ALL, "SELECT account.id, username FROM account, account_banned WHERE account.id = account_banned.id AND active = 1 GROUP BY account.id", CONNECTION_BOTH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_BANNED_BY_USERNAME, "SELECT account.id, username FROM account, account_banned WHERE account.id = account_banned.id AND active = 1 AND username LIKE CONCAT('%%', ?, '%%') GROUP BY account.id", CONNECTION_BOTH);
    PrepareStatement(LOGIN_DEL_ACCOUNT_BANNED, "DELETE FROM account_banned WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME, "SELECT id, hwid FROM account WHERE username = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_LIST_BY_NAME, "SELECT id, username FROM account WHERE username = ?", CONNECTION_BOTH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME, "SELECT a.id, a.sessionkey, ba.last_ip, ba.locked, ba.lock_country, a.expansion, a.mutetime, ba.locale, a.recruiter, a.os, ba.id, aa.gmLevel, a.AtAuthFlag, bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id, a.hwid FROM account a LEFT JOIN account r ON a.id = r.recruiter LEFT JOIN battlenet_accounts ba ON a.battlenet_account = ba.id LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?) LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1 WHERE a.username = ? ORDER BY aa.RealmID DESC LIMIT 1", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_ACCOUNT_LIST_BY_EMAIL, "SELECT a.id, a.username FROM account a LEFT JOIN battlenet_accounts ba on (ba.id = a.battlenet_account) WHERE ba.email = ?", CONNECTION_BOTH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_BY_IP, "SELECT id, username FROM account WHERE last_ip = ?", CONNECTION_BOTH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_BY_ID, "SELECT 1 FROM account WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_INS_IP_BANNED, "INSERT INTO ip_banned VALUES (?, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_IP_NOT_BANNED, "DELETE FROM ip_banned WHERE ip = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_ACCOUNT_BANNED, "INSERT INTO account_banned VALUES (?, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+?, ?, ?, 1)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ACCOUNT_BANNED, "UPDATE account_banned SET unbandate = unbandate + ? WHERE id = ? AND active = 1", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED, "UPDATE account_banned SET active = 0 WHERE id = ? AND active != 0", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_REALM_CHARACTERS_BY_REALM, "DELETE FROM realmcharacters WHERE acctid = ? AND realmid = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_REALM_CHARACTERS, "DELETE FROM realmcharacters WHERE acctid = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_REALM_CHARACTERS, "REPLACE INTO realmcharacters (numchars, acctid, realmid) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_SUM_REALM_CHARACTERS, "SELECT SUM(numchars) FROM realmcharacters WHERE acctid = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_ACCOUNT, "INSERT INTO account(username, sha_pass_hash, joindate, battlenet_account, battlenet_index) VALUES(?, ?, NOW(), ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_REALM_CHARACTERS_INIT, "INSERT INTO realmcharacters (realmid, acctid, numchars) SELECT realmlist.id, account.id, 0 FROM realmlist, account LEFT JOIN realmcharacters ON acctid=account.id WHERE acctid IS NULL", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_EXPANSION, "UPDATE account SET expansion = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ACCOUNT_LOCK, "UPDATE account SET locked = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_LOG, "INSERT INTO logs (time, realm, type, level, string) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_USERNAME, "UPDATE account SET v = 0, s = 0, username = ?, sha_pass_hash = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_PASSWORD, "UPDATE account SET v = 0, s = 0, sha_pass_hash = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_MUTE_TIME, "UPDATE account SET mutetime = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_LAST_IP, "UPDATE account SET last_ip = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ACCOUNT_ONLINE, "UPDATE account SET online = 1 WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_UPTIME_PLAYERS, "UPDATE uptime SET uptime = ?, maxplayers = ? WHERE realmid = ? AND starttime = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_OLD_LOGS, "DELETE FROM logs WHERE (time + ?) < ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_ACCOUNT_ACCESS, "DELETE FROM account_access WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_DEL_ACCOUNT_ACCESS_BY_REALM, "DELETE FROM account_access WHERE id = ? AND (RealmID = ? OR RealmID = -1)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_ACCOUNT_ACCESS, "INSERT INTO account_access (id,gmlevel,RealmID) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME, "SELECT id FROM account WHERE username = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_GET_ACCOUNT_ACCESS_GMLEVEL, "SELECT gmlevel FROM account_access WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_GET_GMLEVEL_BY_REALMID, "SELECT gmlevel FROM account_access WHERE id = ? AND (RealmID = ? OR RealmID = -1)", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_GET_USERNAME_BY_ID, "SELECT username FROM account WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_CHECK_PASSWORD, "SELECT 1 FROM account WHERE id = ? AND sha_pass_hash = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_CHECK_PASSWORD_BY_NAME, "SELECT 1 FROM account WHERE username = ? AND sha_pass_hash = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_PINFO, "SELECT a.username, aa.gmlevel, a.email, a.last_ip, DATE_FORMAT(a.last_login, '%Y-%m-%d %T'), a.mutetime, ba.id AS bid FROM account a LEFT JOIN account_access aa ON (a.id = aa.id AND (aa.RealmID = ? OR aa.RealmID = -1)) JOIN battlenet_accounts ba ON ba.id = a.battlenet_account WHERE a.id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_PINFO_BANS, "SELECT unbandate, bandate = unbandate, bannedby, banreason FROM account_banned WHERE id = ? AND active ORDER BY bandate ASC LIMIT 1", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_GM_ACCOUNTS, "SELECT a.username, aa.gmlevel FROM account a, account_access aa WHERE a.id=aa.id AND aa.gmlevel >= ? AND (aa.realmid = -1 OR aa.realmid = ?)", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_INFO, "SELECT a.username, a.last_ip, aa.gmlevel, a.expansion FROM account a LEFT JOIN account_access aa ON (a.id = aa.id) WHERE a.id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_ACCESS_GMLEVEL_TEST, "SELECT 1 FROM account_access WHERE id = ? AND gmlevel > ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_ACCESS, "SELECT a.id, aa.gmlevel, aa.RealmID FROM account a LEFT JOIN account_access aa ON (a.id = aa.id) WHERE a.username = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_ACCOUNT_WHOIS, "SELECT username, email, last_ip FROM account WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_REALMLIST_SECURITY_LEVEL, "SELECT allowedSecurityLevel from realmlist WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_DEL_ACCOUNT, "DELETE FROM account WHERE id = ?", CONNECTION_ASYNC);

    PrepareStatement(LOGIN_SET_DUMP, "INSERT INTO `transferts` (`account`, `perso_guid`, `from`, `to`, `state`, `dump`) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_DUMP, "UPDATE transferts SET dump = ? ,state = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_ADD_TRANSFERTS_LOGS, "INSERT INTO transferts_logs (`id`, `account`, `perso_guid`, `from`, `to`, `dump`, `toacc`, `newguid`, `transferId`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);

#define BnetAccountInfo "ba.id, ba.email, ba.locked, ba.lock_country, ba.last_ip, ba.failed_logins, bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, bab.unbandate = bab.bandate, ba.activate, ba.access_ip"
#define BnetGameAccountInfo "a.id, a.username, ab.unbandate, ab.unbandate = ab.bandate, aa.gmlevel"

    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_INFO, "SELECT " BnetAccountInfo ", " BnetGameAccountInfo " FROM battlenet_accounts ba LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account a ON ba.id = a.battlenet_account LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1 LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID = -1 WHERE ba.email = ? AND ba.sha_pass_hash = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_INFO_BY_ACC, "SELECT " BnetAccountInfo ", " BnetGameAccountInfo " FROM battlenet_accounts ba LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account a ON ba.id = a.battlenet_account LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1 LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID = -1 WHERE a.username = ? AND ba.sha_pass_hash = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_BNET_LAST_LOGIN_INFO, "UPDATE battlenet_accounts SET last_ip = ?, last_login = NOW(), locale = ?, failed_logins = 0, os = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_BNET_ACCOUNT, "INSERT INTO battlenet_accounts (`email`,`sha_pass_hash`) VALUES (?, ?)", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_EMAIL_BY_ID, "SELECT email FROM battlenet_accounts WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_ID_BY_EMAIL, "SELECT id FROM battlenet_accounts WHERE email = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_BNET_PASSWORD, "UPDATE battlenet_accounts SET sha_pass_hash = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_BNET_CHECK_PASSWORD, "SELECT 1 FROM battlenet_accounts WHERE id = ? AND sha_pass_hash = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_BNET_ACCOUNT_LOCK, "UPDATE battlenet_accounts SET locked = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_BNET_ACCOUNT_LOCK_CONTRY, "UPDATE battlenet_accounts SET lock_country = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_ID_BY_GAME_ACCOUNT, "SELECT battlenet_account FROM account WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_ACCOUNT_EMAIL_BY_ACC, "SELECT ba.email FROM battlenet_accounts ba JOIN account a ON ba.id = a.battlenet_account WHERE a.username = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_BNET_GAME_ACCOUNT_LINK, "UPDATE account SET battlenet_account = ?, battlenet_index = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_BNET_MAX_ACCOUNT_INDEX, "SELECT MAX(battlenet_index) FROM account WHERE battlenet_account = ?", CONNECTION_SYNCH);

    PrepareStatement(LOGIN_UPD_AT_AUTH_FLAG, "UPDATE account SET AtAuthFlag = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_BATTLEPAY_COINS, "SELECT coins FROM account WHERE id = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_BATTLEPAY_CHANGE_COINS_COUNT, "UPDATE account SET coins = coins - ? WHERE id = ?", CONNECTION_ASYNC);

    PrepareStatement(LOGIN_UPD_ACCOUNT_INFO_CONTINUED_SESSION, "UPDATE account SET sessionkey = ? WHERE id = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION, "SELECT username, sessionkey FROM account WHERE id = ?", CONNECTION_ASYNC);

    PrepareStatement(LOGIN_UPD_BNET_GAME_ACCOUNT_LOGIN_INFO, "UPDATE account SET sessionkey = ?, last_ip = ?, last_login = NOW(), locale = ?, failed_logins = 0, os = ? WHERE username = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_LAST_PLAYER_CHARACTERS, "SELECT lpc.accountId, lpc.region, lpc.battlegroup, lpc.realmId, lpc.characterName, lpc.characterGUID, lpc.lastPlayedTime FROM account_last_played_character lpc LEFT JOIN account a ON lpc.accountId = a.id WHERE a.battlenet_account = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_DEL_BNET_LAST_PLAYER_CHARACTERS, "DELETE FROM account_last_played_character WHERE accountId = ? AND region = ? AND battlegroup = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_BNET_LAST_PLAYER_CHARACTERS, "INSERT INTO account_last_played_character (accountId, region, battlegroup, realmId, characterName, characterGUID, lastPlayedTime) VALUES (?,?,?,?,?,?,?)", CONNECTION_ASYNC);
    
    PrepareStatement(LOGIN_SEL_BNET_CHARACTER_COUNTS_BY_BNET_ID, "SELECT rc.acctid, rc.numchars, r.id, r.Region, r.Battlegroup FROM realmcharacters rc INNER JOIN realmlist r ON rc.realmid = r.id LEFT JOIN account a ON rc.acctid = a.id WHERE a.battlenet_account = ?", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_SEL_BNET_CHARACTER_COUNTS_BY_ACCOUNT_ID, "SELECT rc.acctid, rc.numchars, r.id, r.Region, r.Battlegroup FROM realmcharacters rc INNER JOIN realmlist r ON rc.realmid = r.id WHERE rc.acctid = ?", CONNECTION_ASYNC);
    
    PrepareStatement(LOGIN_SEL_ACCOUNT_IP_ACCESS, "select min, max from account_ip_access WHERE pid = ? AND enable = 1", CONNECTION_SYNCH);
    
    PrepareStatement(LOGIN_SELECT_DONATE_TOKEN, "SELECT `balans` from battlenet_accounts WHERE id = ?;", CONNECTION_SYNCH);
    PrepareStatement(LOGIN_UPD_DESTROY_DONATE_TOKEN, "UPDATE battlenet_accounts SET balans = balans - ? WHERE id = ?;", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ADD_DONATE_TOKEN, "UPDATE battlenet_accounts SET balans = balans + ? WHERE id = ?;", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`, `bonus`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, (select `bonus` from `store_products` where id = ?));", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG_SERVICE, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, (select `id` from `store_products` where `item` = ?));", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG_BONUS, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`, `bonus`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, (select `id` from `store_products` where `item` = ?), ?);", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG_ADDITIONAL, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`, `bonus`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_HISTORY_GUID, "UPDATE `store_history` SET item_guid = ? WHERE id = ? AND item = ?;", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_HISTORY_STATUS, "UPDATE `store_history` SET `status` = ? WHERE `item_guid` = ? and `realm` = ? and (`status` = 0 or `status` = 6)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_HISTORY_STATUS_AND_GUID_BY_STATUS, "UPDATE `store_history` SET `status` = ?, `item_guid` = ? WHERE `item_guid` = ? and `realm` = ? and `status` = ?", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_HISTORY_RETURN, "UPDATE `store_history` SET `status` = 7, dt_return = NOW() WHERE `item_guid` = ? and `realm` = ? and (`status` = 0 or `status` = 6)", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG_SERVICE_ART, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`, art_level) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, (select `id` from `store_products` where `item` = ?), ?);", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_INS_STORE_ADD_ITEM_LOG_GUILD_RENAME, "INSERT INTO `store_history` (`realm`, `account`, `bnet_account`, `char_guid`, `item_guid`, `item`, `count`, `token`, `char_level`, `product`, `guild_name`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, (select `id` from `store_products` where `item` = ?), ?);", CONNECTION_ASYNC);

    // 0 - по умолчанию, игрок получил предмет
    // 1 - удалён
    // 2 - продан
    // 3 - использован и удален
    // 4 - распылён
    // 5 - в хранилище (продолжаем следить при возврате)
    // 6 - использован, но не удален (многоразовый?) (продолжаем следить)
    // 7 - возвращен

    PrepareStatement(LOGIN_SEL_ACCOUNT_CHARACTER_TEMPLATE, "SELECT `id`, `level`, `iLevel`, `money`, `artifact`, `transferId`, `templateId` FROM account_character_template WHERE account = ? AND realm = ? AND charGuid = 0", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_ACCOUNT_CHARACTER_TEMPLATE, "UPDATE `account_character_template` SET charGuid = ? WHERE id = ?;", CONNECTION_ASYNC);
    PrepareStatement(LOGIN_UPD_TRANSFER_REQUESTS, "UPDATE `transfer_requests` SET guid = ?, `status` = '0', `char_class` = ?, `char_faction` = ? WHERE id = ?", CONNECTION_ASYNC);
}
