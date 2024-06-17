#include "ButWhy.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"

bool ButWhyCommandScript::CanExecuteCommand(ChatHandler& handler, std::string_view cmdV)
{
    if (!sConfigMgr->GetOption<bool>("ButWhy.Enable", false))
    {
        return true;
    }

    auto player = handler.GetPlayer();

    // Only process player commands.
    if (!player)
    {
        return true;
    }

    if (!ButWhy::ShouldRequestReason(player))
    {
        return true;
    }

    auto guid = player->GetGUID();
    auto cmd = std::string(cmdV.data());

    if (!ButWhy::HasState(guid))
    {
        ButWhy::SetupState(guid);
    }

    if (ButWhy::IsReasoning(guid))
    {
        if (ButWhy::GetLastCommand(guid) == cmd && ButWhy::GetLastCommandResolved(guid))
        {
            ButWhy::SetLastCommandResolved(guid, false);
            ButWhy::SetIsReasoning(guid, false);
            return true;
        }

        ButWhy::Notify(player, "You need to provide a reason for the previous command before executing a new one.");
        return false;
    }

    ButWhy::SetLastCommand(guid, cmd);
    ButWhy::SetIsReasoning(guid, true);
    ButWhy::Notify(player, "You need to provide a reason for executing this command.");

    return false;
}

bool ButWhyPlayerScript::CanPlayerUseChat(Player* player, uint32 /*type*/, uint32 /*language*/, std::string& msg)
{
    if (!sConfigMgr->GetOption<bool>("ButWhy.Enable", false))
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    auto guid = player->GetGUID();

    if (!ButWhy::IsReasoning(guid))
    {
        return true;
    }

    if (msg.empty())
    {
        return false;
    }

    auto handler = ChatHandler(player->GetSession());
    auto command = ButWhy::GetLastCommand(guid);

    ButWhy::Notify(player, Acore::StringFormatFmt("The reason for executing this command was logged, executing command '{}'..", command));
    ButWhy::LogReason(player, command, msg);

    ButWhy::SetLastCommandResolved(guid, true);

    auto result = Acore::Impl::ChatCommands::ChatCommandNode::TryExecuteCommand(handler, command);
    if (!result)
    {
        ButWhy::Notify(player, "Failed to execute the command.");
        return false;
    }

    return false;
}

void ButWhyPlayerScript::OnLogout(Player* player)
{
    if (!sConfigMgr->GetOption<bool>("ButWhy.Enable", false))
    {
        return;
    }

    if (!player)
    {
        return;
    }

    auto guid = player->GetGUID();
    if (!ButWhy::HasState(guid))
    {
        return;
    }

    ButWhy::ClearState(guid);
}

bool ButWhy::HasState(ObjectGuid guid)
{
    return states.find(guid) != states.end();
}

void ButWhy::SetupState(ObjectGuid guid)
{
    if (HasState(guid))
    {
        return;
    }

    WhyState state;
    state.isReasoning = false;
    state.lastCommand = "";
    state.lastCommandResolved = false;

    states.emplace(guid, state);
}

void ButWhy::ClearState(ObjectGuid guid)
{
    if (!HasState(guid))
    {
        return;
    }

    auto it = states.find(guid);
    it->second.isReasoning = false;
    it->second.lastCommand = "";
    it->second.lastCommandResolved = false;
}

bool ButWhy::IsReasoning(ObjectGuid guid)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return false;
    }

    return it->second.isReasoning;
}

void ButWhy::SetIsReasoning(ObjectGuid guid, bool state)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return;
    }

    it->second.isReasoning = state;
}

void ButWhy::Notify(Player* player, std::string message)
{
    player->SendSystemMessage(Acore::StringFormatFmt("|cffFF0000[ButWhy]:|cffFFFFFF {}|r", message));
}

void ButWhy::LogReason(Player* player, std::string command, std::string reason)
{
    auto target = player->GetTarget();
    auto targetName = player->GetName();

    if (target)
    {
        auto targetUnit = ObjectAccessor::GetUnit(*player, target);
        if (targetUnit)
        {
            targetName = targetUnit->GetName();
        }
    }

    LOG_INFO("module.butwhy", Acore::StringFormatFmt("Player '{}' executed command '{}' on target '{}' with reason '{}'.", player->GetName(), command, targetName, reason));
}

bool ButWhy::ShouldRequestReason(Player* player)
{
    if (!sConfigMgr->GetOption<bool>("ButWhy.Enable", false))
    {
        return false;
    }

    auto securityLevel = player->GetSession()->GetSecurity();
    switch (securityLevel)
    {
    case SEC_PLAYER:
        if (sConfigMgr->GetOption<bool>("ButWhy.Ask.Player", false))
        {
            return true;
        }
        break;

    case SEC_MODERATOR:
        if (sConfigMgr->GetOption<bool>("ButWhy.Ask.Moderator", true))
        {
            return true;
        }
        break;

    case SEC_GAMEMASTER:
        if (sConfigMgr->GetOption<bool>("ButWhy.Ask.GameMaster", true))
        {
            return true;
        }
        break;

    case SEC_ADMINISTRATOR:
        if (sConfigMgr->GetOption<bool>("ButWhy.Ask.Administrator", false))
        {
            return true;
        }
        break;

    case SEC_CONSOLE:
        return false;
    }

    return false;
}

std::string ButWhy::GetLastCommand(ObjectGuid guid)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return "";
    }

    return it->second.lastCommand;
}

void ButWhy::SetLastCommand(ObjectGuid guid, std::string cmd)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return;
    }

    it->second.lastCommand = cmd;
}

bool ButWhy::GetLastCommandResolved(ObjectGuid guid)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return false;
    }

    return it->second.lastCommandResolved;
}

void ButWhy::SetLastCommandResolved(ObjectGuid guid, bool state)
{
    auto it = states.find(guid);
    if (it == states.end())
    {
        return;
    }

    it->second.lastCommandResolved = state;
}

void SC_AddButWhyScripts()
{
    new ButWhyCommandScript();
    new ButWhyPlayerScript();
}
