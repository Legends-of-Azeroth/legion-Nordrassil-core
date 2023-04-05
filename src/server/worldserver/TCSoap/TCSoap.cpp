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

#include "TCSoap.h"
#include "soapH.h"
#include "soapStub.h"
#include "World.h"
#include "AccountMgr.h"
#include "Log.h"

SOAPCommand::SOAPCommand() : m_success(false) { }

void SOAPCommand::appendToPrintBuffer(const char* msg)
{
    m_printBuffer += msg;
}

void SOAPCommand::setCommandSuccess(bool val)
{
    m_success = val;
    finishedPromise.set_value();
}

bool SOAPCommand::hasCommandSucceeded() const
{
    return m_success;
}

void TCSoapThread(const std::string& host, uint16 port)
{
    struct soap soap;
    soap_init(&soap);
    soap_set_imode(&soap, SOAP_C_UTFSTRING);
    soap_set_omode(&soap, SOAP_C_UTFSTRING);

    // check every 3 seconds if world ended
    soap.accept_timeout = 3;
    soap.recv_timeout = 5;
    soap.send_timeout = 5;
    if (!soap_valid_socket(soap_bind(&soap, host.c_str(), port, 100)))
    {
        TC_LOG_ERROR(LOG_FILTER_WORLDSERVER, "Couldn't bind to %s:%d", host.c_str(), port);
        exit(-1);
    }

    TC_LOG_INFO(LOG_FILTER_WORLDSERVER, "Bound to http://%s:%d", host.c_str(), port);

    while (!World::IsStopped())
    {
        if (!soap_valid_socket(soap_accept(&soap)))
            continue;   // ran into an accept timeout

        TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: accepted connection from IP=%d.%d.%d.%d", (int)(soap.ip >> 24) & 0xFF, (int)(soap.ip >> 16) & 0xFF, (int)(soap.ip >> 8) & 0xFF, (int)soap.ip & 0xFF);
        
        auto thread_soap = soap_copy(&soap);// make a safe copy
        process_message(thread_soap);
    }

    soap_done(&soap);
}

void process_message(struct soap* soap_message)
{
    TC_LOG_TRACE(LOG_FILTER_NETWORKIO, "SOAPWorkingThread::process_message");

    soap_serve(soap_message);
    soap_destroy(soap_message); // dealloc C++ data
    soap_end(soap_message); // dealloc data and clean up
    soap_done(soap_message); // detach soap struct
    free(soap_message);
}
/*
Code used for generating stubs:

int ns1__executeCommand(char* command, char** result);
*/
int ns1__executeCommand(soap* soap, char* command, char** result)
{
    // security check
    if (!soap->userid || !soap->passwd)
    {
        TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: Client didn't provide login information");
        return 401;
    }

    uint32 accountId = AccountMgr::GetId(soap->userid);
    if (!accountId)
    {
        TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: Client used invalid username '%s'", soap->userid);
        return 401;
    }

    if (!AccountMgr::CheckPassword(accountId, soap->passwd))
    {
        TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: invalid password for account '%s'", soap->userid);
        return 401;
    }

    if (AccountMgr::GetSecurity(accountId) < SEC_ADMINISTRATOR)
    {
        TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: %s's gmlevel is too low", soap->userid);
        return 403;
    }

    if (!command || !*command)
        return soap_sender_fault(soap, "Command mustn't be empty", "The supplied command was an empty string");

    TC_LOG_DEBUG(LOG_FILTER_NETWORKIO, "TCSoap: got command '%s'", command);
    SOAPCommand connection;

    // commands are executed in the world thread. We have to wait for them to be completed
    {
        // CliCommandHolder will be deleted from world, accessing after queueing is NOT safe
        auto cmd = new CliCommandHolder(&connection, command, &SOAPCommand::print, &SOAPCommand::commandFinished);
        sWorld->QueueCliCommand(cmd);
    }

    // Wait until the command has finished executing
    connection.finishedPromise.get_future().wait();

    // The command has finished executing already
    char* printBuffer = soap_strdup(soap, connection.m_printBuffer.c_str());
    if (connection.hasCommandSucceeded())
    {
        *result = printBuffer;
        return SOAP_OK;
    }

    return soap_sender_fault(soap, printBuffer, printBuffer);
}

void SOAPCommand::print(void* callbackArg, const char* msg)
{
    ((SOAPCommand*)callbackArg)->appendToPrintBuffer(msg);
}

void SOAPCommand::commandFinished(void* soapconnection, bool success)
{
    SOAPCommand* con = (SOAPCommand*)soapconnection;
    con->setCommandSuccess(success);
}

////////////////////////////////////////////////////////////////////////////////
//
//  Namespace Definition Table
//
////////////////////////////////////////////////////////////////////////////////

struct Namespace namespaces[] =
{ 
    { "SOAP-ENV",   "http://schemas.xmlsoap.org/soap/envelope/",    nullptr,                                    nullptr }, // must be first
    { "SOAP-ENC",   "http://schemas.xmlsoap.org/soap/encoding/",    nullptr,                                    nullptr }, // must be second
    { "xsi",        "http://www.w3.org/1999/XMLSchema-instance",    "http://www.w3.org/*/XMLSchema-instance",   nullptr },
    { "xsd",        "http://www.w3.org/1999/XMLSchema",             "http://www.w3.org/*/XMLSchema",            nullptr },
    { "ns1",        "urn:TC",                                       nullptr,                                    nullptr }, // "ns1" namespace prefix
    { nullptr,      nullptr,                                        nullptr,                                    nullptr }
};
