#include "user.hpp"

// ============================
// ======== SteamUser =========
// ============================

namespace {

class CallbackListener;
CallbackListener *user_listener = nullptr;
int user_ref = LUA_NOREF;

class CallbackListener {
  private:
    STEAM_CALLBACK(CallbackListener, OnAuthSessionTicketResponse, GetAuthSessionTicketResponse_t);
};

void CallbackListener::OnAuthSessionTicketResponse(GetAuthSessionTicketResponse_t *data) {
    if (data == nullptr) {
        return;
    }
    lua_State *L = luasteam::global_lua_state;
    if (!lua_checkstack(L, 4)) {
        return;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, user_ref);
    lua_getfield(L, -1, "onAuthSessionTicketResponse");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
    } else {
        lua_createtable(L, 0, 1);
        lua_pushinteger(L, data->m_hAuthTicket);
        lua_setfield(L, -2, "handle");
        lua_pushstring(L, steam_result_code[data->m_eResult]);
        lua_setfield(L, -2, "result");
        lua_call(L, 1, 0);
        lua_pop(L, 1);
    }
}

} // namespace

// int GetPlayerSteamLevel();
EXTERN int luasteam_getPlayerSteamLevel(lua_State *L) {
    lua_pushnumber(L, SteamUser()->GetPlayerSteamLevel());
    return 1;
}

// CSteamID GetSteamID();
EXTERN int luasteam_getSteamID(lua_State *L) {
    luasteam::pushuint64(L, SteamUser()->GetSteamID().ConvertToUint64());
    return 1;
}

//HAuthTicket GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket )
EXTERN int luasteam_getAuthSessionTicket(lua_State *L) {
    uint32 *pcbTicket = nullptr;
    void *pTicket = nullptr;
    HAuthTicket ticket = SteamUser()->GetAuthSessionTicket(pTicket, 1024, pcbTicket);

    lua_pushinteger(L, ticket);
    return 1;
}

//void CancelAuthTicket( HAuthTicket hAuthTicket )
EXTERN int luasteam_cancelAuthTicket(lua_State *L) {
    HAuthTicket ticket = luaL_checkinteger(L, 1);
    SteamUser()->CancelAuthTicket(ticket);
    return 0;
}

namespace luasteam {

void add_user(lua_State *L) {
    lua_createtable(L, 0, 4);
    add_func(L, "getPlayerSteamLevel", luasteam_getPlayerSteamLevel);
    add_func(L, "getSteamID", luasteam_getSteamID);
    add_func(L, "getAuthSessionTicket", luasteam_getAuthSessionTicket);
    add_func(L, "cancelAuthTicket", luasteam_cancelAuthTicket);
    lua_pushvalue(L, -1);
    user_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_setfield(L, -2, "user");
}

void init_user(lua_State *L) { user_listener = new CallbackListener(); }

void shutdown_user(lua_State *L) {
    luaL_unref(L, LUA_REGISTRYINDEX, user_ref);
    user_ref = LUA_NOREF;
    delete user_listener;
    user_listener = nullptr;
}

} // namespace luasteam
