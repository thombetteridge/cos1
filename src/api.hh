#pragma once

#include "base.hh"

C_LINKAGE_BEGIN

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void l_register_api(lua_State *L);

int l_draw_line(lua_State *L);
int l_draw_circle(lua_State *L);
int l_get_mouse_position(lua_State *L);
int l_is_mouse_button_down(lua_State *L);
int l_add_point(lua_State *L);
int l_is_mouse_button_pressed(lua_State *L);
int l_is_mouse_button_released(lua_State *L);
int l_gui_slider(lua_State *L);
int l_gui_button(lua_State *L);
int l_draw_text(lua_State *L);

C_LINKAGE_END
