#ifndef LUASTEAM_REMOTESTORAGE_HPP
#define LUASTEAM_REMOTESTORAGE_HPP

#include "common.hpp"

namespace luasteam {
void add_remotestorage(lua_State *L);
void init_remotestorage(lua_State *L);
void shutdown_remotestorage(lua_State *L);
} // namespace luasteam

#endif