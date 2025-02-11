#include "remotestorage.hpp"

// ============================
// ===== SteamRemoteStorage ====
// ============================

namespace {
int remotestorage_ref = LUA_NOREF;
}

// bool IsCloudEnabledForAccount();
EXTERN int luasteam_isCloudEnabledForAccount(lua_State *L) {
    bool enabled = SteamRemoteStorage()->IsCloudEnabledForAccount();
    lua_pushboolean(L, enabled);
    return 1;
}

namespace luasteam {

void add_remotestorage(lua_State *L) {
    lua_createtable(L, 0, 1);
    add_func(L, "isCloudEnabledForAccount", luasteam_isCloudEnabledForAccount);
    lua_pushvalue(L, -1);
    remotestorage_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_setfield(L, -2, "remoteStorage");
}

void init_remotestorage(lua_State *L) {
    // No initialization needed for now
}

void shutdown_remotestorage(lua_State *L) {
    luaL_unref(L, LUA_REGISTRYINDEX, remotestorage_ref);
    remotestorage_ref = LUA_NOREF;
}

} // namespace luasteam