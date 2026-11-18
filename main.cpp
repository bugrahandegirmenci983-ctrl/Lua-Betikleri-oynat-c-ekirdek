#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

// extern "C" KALDIRILDI! Luau header'lari kendi icinde extern "C" iceriyor.
#include "C:/my-executor/extern/luau/VM/include/lua.h"
#include "C:/my-executor/extern/luau/VM/include/lualib.h"
#include "C:/my-executor/extern/luau/Compiler/include/Luau/Compiler.h"

// print fonksiyonu
static int l_myprint(lua_State* L) {
    int n = lua_gettop(L);
    std::string out;
    for (int i = 1; i <= n; ++i) {
        const char* s = luaL_tolstring(L, i, nullptr);
        out += s;
        if (i < n) out += " ";
        lua_pop(L, 1);
    }
    std::printf("[out] %s\n", out.c_str());
    return 0;
}

// getgenv
static int l_getgenv(lua_State* L) {
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    return 1;
}

// Luau compile + yükle
static int run_string(lua_State* L, const char* code, const char* name) {
    try {
        std::string bytecode = Luau::compile(code);
        if (luau_load(L, name, bytecode.data(), bytecode.size(), 0) != 0) {
            return -1;
        }
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
            return -1;
        }
        return 0;
    } catch (const std::exception& e) {
        lua_pushstring(L, e.what());
        return -1;
    }
}

// Dosyadan çalıştır
static int run_file(lua_State* L, const char* path) {
    FILE* f = std::fopen(path, "rb");
    if (!f) {
        std::fprintf(stderr, "[hata] Dosya acilamadi: %s\n", path);
        return -1;
    }
    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    std::string buf(size, '\0');
    std::fread(buf.data(), 1, size, f);
    std::fclose(f);

    return run_string(L, buf.c_str(), path);
}

int main(int argc, char** argv) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, l_myprint, "print");
    lua_setglobal(L, "print");
    lua_pushcfunction(L, l_getgenv, "getgenv");
    lua_setglobal(L, "getgenv");

    if (argc > 1) {
        if (run_file(L, argv[1]) != 0) {
            std::fprintf(stderr, "[hata] %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        std::printf("REPL basladi. Cikmak icin 'exit' yaz.\n");
        std::string line;
        while (true) {
            std::printf("> ");
            std::fflush(stdout);
            if (!std::getline(std::cin, line)) break;
            if (line == "exit") break;
            if (run_string(L, line.c_str(), "=stdin") != 0) {
                std::fprintf(stderr, "[hata] %s\n", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    }

    lua_close(L);
    return 0;
}