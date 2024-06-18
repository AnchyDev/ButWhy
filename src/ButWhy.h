#ifndef MODULE_BUT_WHY
#define MODULE_BUT_WHY

#include "ScriptMgr.h"

namespace ButWhy
{
    struct WhyState
    {
        bool isReasoning;
        std::string lastCommand;
        bool lastCommandResolved;
    };

    std::map<ObjectGuid, WhyState> states;

    bool HasState(ObjectGuid /*guid*/);
    void SetupState(ObjectGuid /*guid*/);
    void ClearState(ObjectGuid /*guid*/);

    bool IsReasoning(ObjectGuid /*guid*/);
    void SetIsReasoning(ObjectGuid /*guid*/, bool /*state*/);

    std::string GetLastCommand(ObjectGuid /*guid*/);
    void SetLastCommand(ObjectGuid /*guid*/, std::string /*cmd*/);

    bool GetLastCommandResolved(ObjectGuid /*guid*/);
    void SetLastCommandResolved(ObjectGuid /*guid*/, bool /*state*/);

    void Notify(Player* /*guid*/, std::string /*message*/);
    void LogReason(Player* /*player*/, std::string /*command*/, std::string /*reason*/);

    bool ShouldRequestReason(Player* /*player*/);
}

class ButWhyCommandScript : public AllCommandScript
{
public:
    ButWhyCommandScript() : AllCommandScript("ButWhyCommandScript", { ALLCOMMANDHOOK_ON_TRY_EXECUTE_COMMAND }) { }

private:
    bool OnTryExecuteCommand(ChatHandler& /*handler*/, std::string_view /*cmdStr*/) override;
};

class ButWhyPlayerScript : public PlayerScript
{
public:
    ButWhyPlayerScript() : PlayerScript("ButWhyPlayerScript", { PLAYERHOOK_CAN_PLAYER_USE_CHAT, PLAYERHOOK_ON_LOGOUT }) { }

private:
    bool CanPlayerUseChat(Player* /*player*/, uint32 /*type*/, uint32 /*language*/, std::string& /*msg*/) override;
    void OnLogout(Player* /*player*/) override;
};

#endif // MODULE_BUT_WHY
