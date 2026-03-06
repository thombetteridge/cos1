// main.c
#include "base.hh"

C_LINKAGE_BEGIN
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
C_LINKAGE_END

#include <gc/gc.h>

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <stdio.h>
#include <stdlib.h>

#include "api.hh"

// ─────────────────────────────────────────────────────────────────────────────
// BDWGC + Lua custom allocator (single shared heap)
static void *bdwgc_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
   (void)ud;
   (void)osize;
   if (nsize == 0) {
      // GC_free(ptr);
      return NULL;
   }
   if (ptr == NULL) {
      return GC_malloc(nsize);
   }
   return GC_realloc(ptr, nsize);
}

// Main

i32 main(void)
{
   GC_INIT();

   // Create window
   static constexpr i32 screen_width  = 800;
   static constexpr i32 screen_height = 600;
   InitWindow(screen_width, screen_height, "Civil Modelling – raylib + Lua + BDWGC");
   SetTargetFPS(60);

   lua_State *L = lua_newstate(bdwgc_lua_alloc, NULL);
   if (!L) {
      TraceLog(LOG_ERROR, "Failed to create Lua state");
      CloseWindow();
      return 1;
   }

   luaL_openlibs(L);
   l_register_api(L);

   // Load main Lua script (put this file next to the executable)
   if (luaL_dofile(L, "dat/main.lua") != LUA_OK) {
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

