// main.c

#include <gc/gc.h> 
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <stdio.h>
#include <stdlib.h>

// ─────────────────────────────────────────────────────────────────────────────
// BDWGC + Lua custom allocator (single shared heap)
static void *bdwgc_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
  (void)ud;
  (void)osize;
  if (nsize == 0) {
    GC_free(ptr); 
    return NULL;
  }
  if (ptr == NULL)
    return GC_malloc(nsize);
  return GC_realloc(ptr, nsize);
}

// ─────────────────────────────────────────────────────────────────────────────
// Example C functions exposed to Lua

static int l_draw_line(lua_State *L)
{
  float x1  = (float)luaL_checknumber(L, 1);
  float y1  = (float)luaL_checknumber(L, 2);
  float x2  = (float)luaL_checknumber(L, 3);
  float y2  = (float)luaL_checknumber(L, 4);
  Color col = {(unsigned char)luaL_optnumber(L, 5, 255),
               (unsigned char)luaL_optnumber(L, 6, 255),
               (unsigned char)luaL_optnumber(L, 7, 255),
               255};
  DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2.0f, col);
  return 0;
}

static int l_draw_circle(lua_State *L)
{
  float x      = (float)luaL_checknumber(L, 1);
  float y      = (float)luaL_checknumber(L, 2);
  float radius = (float)luaL_checknumber(L, 3);

  Color col = WHITE; // default

  // Check if we got a table as arg #4
  if (lua_istable(L, 4)) {
    // Indexed style: {r, g, b, a} or {r, g, b} (a=255)
    lua_rawgeti(L, 4, 1);
    col.r = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 4, 2);
    col.g = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 4, 3);
    col.b = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 4, 4);
    col.a = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);

    // Optional fallback: if someone passes named fields anyway
    if (col.r == 255 && col.g == 255 && col.b == 255 && col.a == 255) {
      lua_getfield(L, 4, "r");
      col.r = (unsigned char)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_getfield(L, 4, "g");
      col.g = (unsigned char)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_getfield(L, 4, "b");
      col.b = (unsigned char)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
      lua_getfield(L, 4, "a");
      col.a = (unsigned char)luaL_optnumber(L, -1, 255);
      lua_pop(L, 1);
    }
  }
  // Fallback: individual r,g,b,a arguments
  else if (lua_isnumber(L, 4)) {
    col.r = (unsigned char)luaL_optnumber(L, 4, 255);
    col.g = (unsigned char)luaL_optnumber(L, 5, 255);
    col.b = (unsigned char)luaL_optnumber(L, 6, 255);
    col.a = (unsigned char)luaL_optnumber(L, 7, 255);
  }

  DrawCircleLines((int)x, (int)y, radius, col);
  return 0;
}
static int l_get_mouse_position(lua_State *L)
{
  Vector2 pos = GetMousePosition();
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2;
}

static int l_is_mouse_button_down(lua_State *L)
{
  int button = luaL_checkinteger(L, 1); // 0 = left, 1 = right, 2 = middle
  lua_pushboolean(L, IsMouseButtonDown(button));
  return 1;
}

// Near your other l_ functions
static int l_add_point(lua_State *L)
{
  float x = (float)luaL_checknumber(L, 1);
  float y = (float)luaL_checknumber(L, 2);

  // Just push a tiny table back to Lua {x=, y=}
  lua_newtable(L);
  lua_pushnumber(L, x);
  lua_setfield(L, -2, "x");
  lua_pushnumber(L, y);
  lua_setfield(L, -2, "y");

  return 1;
}

static int l_is_mouse_button_pressed(lua_State *L)
{
  int button = luaL_checkinteger(L, 1); // 0=left, 1=right, 2=middle
  lua_pushboolean(L, IsMouseButtonPressed(button));
  return 1;
}

static int l_is_mouse_button_released(lua_State *L)
{
  int button = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsMouseButtonReleased(button));
  return 1;
}

static int l_gui_slider(lua_State *L)
{
  Rectangle bounds = {
      (float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2),
      (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)};
  float value           = (float)luaL_checknumber(L, 5);
  float minValue        = (float)luaL_checknumber(L, 6);
  float maxValue        = (float)luaL_checknumber(L, 7);
  const char *textLeft  = luaL_optstring(L, 8, "");
  const char *textRight = luaL_optstring(L, 9, "");

  float newValue = GuiSlider(bounds, textLeft, textRight, &value, minValue, maxValue);
  lua_pushnumber(L, newValue);
  return 1;
}

static int l_gui_button(lua_State *L)
{
  Rectangle bounds = {
      (float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2),
      (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)};
  const char *text = luaL_checkstring(L, 5);

  bool pressed = GuiButton(bounds, text);
  lua_pushboolean(L, pressed);
  return 1;
}

static int l_draw_text(lua_State *L)
{
  const char *text = luaL_checkstring(L, 1);
  float x          = (float)luaL_checknumber(L, 2);
  float y          = (float)luaL_checknumber(L, 3);
  float fontSize   = (float)luaL_checknumber(L, 4);

  // Color: accept indexed table {r,g,b,a} like your draw_circle
  Color col = {255, 255, 255, 255}; // default white

  if (lua_istable(L, 5)) {
    lua_rawgeti(L, 5, 1);
    col.r = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 5, 2);
    col.g = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 5, 3);
    col.b = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
    lua_rawgeti(L, 5, 4);
    col.a = (unsigned char)luaL_optnumber(L, -1, 255);
    lua_pop(L, 1);
  }

  DrawText(text, (int)x, (int)y, (int)fontSize, col);
  return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Main

int main(void)
{
  // Initialize Boehm GC
  GC_INIT();
  // Optional: GC_enable_incremental(); for lower pause times

  // Create window
  const int screenWidth  = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Civil Modelling – raylib + Lua + BDWGC");
  // SetTargetFPS(60);

  // Create Lua state using BDWGC allocator
  lua_State *L = lua_newstate(bdwgc_lua_alloc, NULL);
  if (!L) {
    TraceLog(LOG_ERROR, "Failed to create Lua state");
    CloseWindow();
    return 1;
  }

  luaL_openlibs(L);

  // Register C functions to Lua
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

  // Load main Lua script (put this file next to the executable)
  if (luaL_dofile(L, "src/main.lua") != LUA_OK) {
    TraceLog(LOG_ERROR, "Lua error: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
    goto cleanup;
  }

  // Main loop
  while (!WindowShouldClose()) {
    // ────────────────────────────────
    // Call Lua update()
    lua_getglobal(L, "update");
    if (lua_isfunction(L, -1)) {
      if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        TraceLog(LOG_ERROR, "update() error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
      }
    }
    else {
      lua_pop(L, 1);
    }

    BeginDrawing();
    {
      ClearBackground(BLACK);

      // ────────────────────────────────
      // Call Lua draw()
      lua_getglobal(L, "draw");
      if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
          TraceLog(LOG_ERROR, "draw() error: %s", lua_tostring(L, -1));
          lua_pop(L, 1);
        }
      }
      else {
        lua_pop(L, 1);
      }

      // Optional: show some debug info
      DrawFPS(10, 10);
    }
    EndDrawing();
  }

cleanup:
  lua_close(L);
  CloseWindow();
  return 0;
}