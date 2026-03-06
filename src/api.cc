#include "api.hh"
#include "base.hh"

#include "raygui.h"
#include "raylib.h"

// Register C functions to Lua
extern "C" void l_register_api(lua_State *L)
{
   lua_register(L, "draw_line", l_draw_line);
   lua_register(L, "draw_circle", l_draw_circle);
   lua_register(L, "get_mouse_position", l_get_mouse_position);
   lua_register(L, "is_mouse_button_down", l_is_mouse_button_down);
   lua_register(L, "add_point", l_add_point);
   lua_register(L, "gui_slider", l_gui_slider);
   lua_register(L, "draw_text", l_draw_text);
   lua_register(L, "is_mouse_button_pressed", l_is_mouse_button_pressed);
   lua_register(L, "is_mouse_button_released", l_is_mouse_button_released);
   lua_register(L, "gui_slider", l_gui_slider);
   lua_register(L, "gui_button", l_gui_button);
}

extern "C" int l_draw_line(lua_State *L)
{
   f64 const x1 = luaL_checknumber(L, 1);
   f64 const y1 = luaL_checknumber(L, 2);
   f64 const x2 = luaL_checknumber(L, 3);
   f64 const y2 = luaL_checknumber(L, 4);

   f64 const r = luaL_optnumber(L, 5, 255);
   f64 const g = luaL_optnumber(L, 6, 255);
   f64 const b = luaL_optnumber(L, 7, 255);

   Color const col = {(u8)r, (u8)g, (u8)b, 255};

   DrawLineEx(Vector2 {.x = (f32)x1, .y = (f32)y1},
              Vector2 {.x = (f32)x2, .y = (f32)y2},
              2.0f, col);
   return 0;
}

extern "C" int l_draw_circle(lua_State *L)
{
   f64 const x      = luaL_checknumber(L, 1);
   f64 const y      = luaL_checknumber(L, 2);
   f64 const radius = luaL_checknumber(L, 3);

   Color col = WHITE; // default

   // Check if we got a table as arg #4
   if (lua_istable(L, 4)) {
      // Indexed style: {r, g, b, a} or {r, g, b} (a=255)
      lua_rawgeti(L, 4, 1);
      col.r = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 4, 2);
      col.g = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 4, 3);
      col.b = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 4, 4);
      col.a = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);

      // Optional fallback: if someone passes named fields anyway
      if (col.r == 255 && col.g == 255 && col.b == 255 && col.a == 255) {
         lua_getfield(L, 4, "r");
         col.r = (u8)luaL_optnumber(L, -1, 255);
         lua_pop(L, 1);
         lua_getfield(L, 4, "g");
         col.g = (u8)luaL_optnumber(L, -1, 255);
         lua_pop(L, 1);
         lua_getfield(L, 4, "b");
         col.b = (u8)luaL_optnumber(L, -1, 255);
         lua_pop(L, 1);
         lua_getfield(L, 4, "a");
         col.a = (u8)luaL_optnumber(L, -1, 255);
         lua_pop(L, 1);
      }
   }
   // Fallback: individual r,g,b,a arguments
   else if (lua_isnumber(L, 4)) {
      col.r = (u8)luaL_optnumber(L, 4, 255);
      col.g = (u8)luaL_optnumber(L, 5, 255);
      col.b = (u8)luaL_optnumber(L, 6, 255);
      col.a = (u8)luaL_optnumber(L, 7, 255);
   }

   DrawCircleLines((int)x, (int)y, (f32)radius, col);
   return 0;
}
extern "C" int l_get_mouse_position(lua_State *L)
{
   Vector2 pos = GetMousePosition();
   lua_pushnumber(L, pos.x);
   lua_pushnumber(L, pos.y);
   return 2;
}
extern "C"

   int
   l_is_mouse_button_down(lua_State *L)
{
   int button = (int)luaL_checkinteger(L, 1); // 0 = left, 1 = right, 2 = middle
   lua_pushboolean(L, IsMouseButtonDown(button));
   return 1;
}

extern "C" int l_add_point(lua_State *L)
{
   f64 const x = luaL_checknumber(L, 1);
   f64 const y = luaL_checknumber(L, 2);

   // Just push a tiny table back to Lua {x=, y=}
   lua_newtable(L);
   lua_pushnumber(L, x);
   lua_setfield(L, -2, "x");
   lua_pushnumber(L, y);
   lua_setfield(L, -2, "y");

   return 1;
}

extern "C" int l_is_mouse_button_pressed(lua_State *L)
{
   int button = (int)luaL_checkinteger(L, 1); // 0=left, 1=right, 2=middle
   lua_pushboolean(L, IsMouseButtonPressed(button));
   return 1;
}

extern "C" int l_is_mouse_button_released(lua_State *L)
{
   int button = (int)luaL_checkinteger(L, 1);
   lua_pushboolean(L, IsMouseButtonReleased(button));
   return 1;
}

extern "C" int l_gui_slider(lua_State *L)
{
   Rectangle bounds = {
      (f32)luaL_checknumber(L, 1), (f32)luaL_checknumber(L, 2),
      (f32)luaL_checknumber(L, 3), (f32)luaL_checknumber(L, 4)};
   f32         value     = (f32)luaL_checknumber(L, 5);
   f32         minValue  = (f32)luaL_checknumber(L, 6);
   f32         maxValue  = (f32)luaL_checknumber(L, 7);
   char const *textLeft  = luaL_optstring(L, 8, "");
   char const *textRight = luaL_optstring(L, 9, "");

   (void)GuiSlider(bounds, textLeft, textRight, &value, minValue, maxValue);
   lua_pushnumber(L, value);
   return 1;
}

extern "C" int l_gui_button(lua_State *L)
{
   Rectangle bounds = {
      (f32)luaL_checknumber(L, 1), (f32)luaL_checknumber(L, 2),
      (f32)luaL_checknumber(L, 3), (f32)luaL_checknumber(L, 4)};
   char const *text = luaL_checkstring(L, 5);

   bool pressed = GuiButton(bounds, text);
   lua_pushboolean(L, pressed);
   return 1;
}

extern "C" int l_draw_text(lua_State *L)
{
   char const *text     = luaL_checkstring(L, 1);
   f32         x        = (f32)luaL_checknumber(L, 2);
   f32         y        = (f32)luaL_checknumber(L, 3);
   f32         fontSize = (f32)luaL_checknumber(L, 4);

   // Color: accept indexed table {r,g,b,a} like your draw_circle
   Color col = {255, 255, 255, 255}; // default white

   if (lua_istable(L, 5)) {
      lua_rawgeti(L, 5, 1);
      col.r = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 5, 2);
      col.g = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 5, 3);
      col.b = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_rawgeti(L, 5, 4);
      col.a = (u8)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
   }

   DrawText(text, (int)x, (int)y, (int)fontSize, col);
   return 0;
}