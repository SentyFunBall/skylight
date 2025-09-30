#include <string>
#include <filesystem>
#include <iostream>
#include <vector>

#include "lua.hpp"
#include "state.h"
#include "funcs.hpp"

#define SCL_IMPL
#include "miniscl.hpp"

using namespace scl::xml;
using std::cout;

// define globals
auto startTime = std::chrono::high_resolution_clock::now();
std::string scriptDir = "./scripts";
std::vector<std::pair<std::string, std::string>> configProperties = {};

lua_State* L = nullptr;
int        tickRef = LUA_NOREF;
int        initRef = LUA_NOREF;

// Simple error handling function
void ErrorAndExit(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

// Simple assert macro for error handling
#define SKL_CASSERT(cond, msg)                                                 \
    if (!(cond)) {                                                             \
        ErrorAndExit(msg);                                                     \
    }

// Define a library of functions to be registered
static const struct luaL_Reg skl_lua[] = {
    {"getTime", Skylight::Lua::LFuncs::gettime},
    {"getConfigProperty", Skylight::Lua::LFuncs::getConfigProperty},
    {"setConfigProperty", Skylight::Lua::LFuncs::setConfigProperty},
    { NULL, NULL } // Sentinel to signal the end of the array
};

// Initialize and start Lua
lua_State* startLua() {
    // Initialize Lua
    lua_State* L = luaL_newstate();

    {
    // Open some functions
    luaL_requiref(L, "_G", luaopen_base, 1);

    lua_pushnil(L);
    lua_setglobal(L, "dofile"); // Disable dofile
    lua_pushnil(L);
    lua_setglobal(L, "loadfile"); // Disable loadfile
    lua_pushnil(L);
    lua_setglobal(L, "loadstring"); // Disable loadstring¸`
    lua_pushnil(L);
    lua_setglobal(L, "error"); // Disable error
    lua_pushnil(L);
    lua_setglobal(L, "assert"); // Disable assert
    lua_pushnil(L);
    lua_setglobal(L, "load"); // Disable load
    lua_pushnil(L);
    lua_setglobal(L, "warn"); // Disable warn
    lua_pushnil(L);
    lua_setglobal(L, "collectgarbage"); // Disable collectgarbage
    lua_pushnil(L);
    lua_setglobal(L, "xpcall"); // Disable xpcall
    lua_pushnil(L);
    lua_setglobal(L, "rawset"); // Disable rawset
    lua_pushnil(L);
    lua_setglobal(L, "rawget"); // Disable rawget
    lua_pushnil(L);
    lua_setglobal(L, "rawequal"); // Disable rawequal
    lua_pushnil(L);
    lua_setglobal(L, "rawlen"); // Disable rawlen
    lua_pushnil(L);
    lua_setglobal(L, "require"); // Disable require (overridden later)
    }

    // Only open specific libraries
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(L, 1);

    luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(L, 1);

    luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(L, 1);

    // Override some functions
    lua_pushcfunction(L, Lfuncs::require);
    lua_setglobal(L, "require");

    // Register 'skylight' library
    luaL_newlib(L, skl_lua);
    lua_setglobal(L, "skylight");

    // Register 'length' into the existing 'table' library
    lua_getglobal(L, "table");              // push the 'table' library on the stack
    if (lua_istable(L, -1)) {
        lua_pushcfunction(L, Lfuncs::tlength);
        lua_setfield(L, -2, "length");        // table.length = Lfuncs::tlength
    }
    lua_pop(L, 1); // pop the 'table' library

    return L;
}

// Load the XML config file and populate configProperties
void loadXML() {
        // Load the mod's config file
    std::string configPath = scriptDir + "/config.xml";
    SKL_CASSERT(std::filesystem::exists(configPath), "Error: Config file not found: " + configPath);

    XmlDocument doc;
    // nasty
    SKL_CASSERT(doc.load_file(configPath.c_str()).code == scl::xml::OK, "Error loading config file");

    // Get the mod properties
    std::vector<XmlElem*> prop = doc.find_children("properties");
    SKL_CASSERT(!prop.empty(), "Error: <properties> element not found in config file");

    XmlAttr* name = prop[0]->find_attr("name");
    XmlAttr* desc = prop[0]->find_attr("desc");
    XmlAttr* author = prop[0]->find_attr("author");
    XmlAttr* version = prop[0]->find_attr("version");

    /* clang-format off */
    cout << "Mod Name: " << (name ? name->data().cstr() : "Unknown") << std::endl;
    cout << "Description: " << (desc ? desc->data().cstr() : "No description") << std::endl;
    cout << "Author: " << (author ? author->data().cstr() : "Unknown") << std::endl;
    cout << "Version: " << (version ? version->data().cstr() : "Unknown") << std::endl;
    /* clang-format on */

    // Get the writeable settings
    std::vector<XmlElem*> writeable = doc.find_children("writeable");
    SKL_CASSERT(!writeable.empty(),
                "Error: <writeable> element not found in config file");
    std::vector<XmlElem*> settings = writeable[0]->find_children("setting");
    for (XmlElem* setting : settings) {
        XmlAttr* key = setting->find_attr("key");
        XmlAttr* value = setting->find_attr("value");
        if (key && value) {
            configProperties.emplace_back(key->data().cstr(),
                                          value->data().cstr());
        }
    }
}

int main(int argc, char* argv[]) {
    startTime = std::chrono::high_resolution_clock::now();
    // process command line arguments
    if (argc > 1) {
        if (argv[1] == std::string("--help") || argv[1] == std::string("-h")) {
            std::cout << "Usage: " << argv[0] << " [script_directory (./scripts by default)]" << std::endl;
            return 0;
        } else {
            scriptDir = argv[1];
        }
    }

    // Various initializations
    scl::init();
    L = startLua();

    loadXML();

    // Load and run the Lua script
    std::string scriptPath = scriptDir + "/main.lua";
    SKL_CASSERT(std::filesystem::exists(scriptPath), "Error: Script file not found: " + scriptPath);

    cout << "The remaining output is the script and related output" << std::endl;

    if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK) {
        std::cerr << "Error running script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // remove error message from stack
    }

    // Get function refs
    lua_getglobal(L, "onModLoad");
    if (lua_isfunction(L, -1)) {
        initRef = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        lua_pop(L, 1); // remove non-function from stack
    }

    lua_getglobal(L, "onTick");
    if (lua_isfunction(L, -1)) {
        tickRef = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        lua_pop(L, 1); // remove non-function from stack
    }

    // Call the init function if it exists
    if (initRef != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, initRef);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            std::cerr << "Error in onModLoad: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1); // remove error message from stack
        }
    }

    // Simulate a few ticks
    for (int i = 0; i < 5; ++i) {
        if (tickRef != LUA_NOREF) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, tickRef);
            lua_pushnumber(L, 0.016); // Simulate ~16ms per tick
            
            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                std::cerr << "Error in onTick: " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1); // remove error message from stack
            }
        }
    }

    scl::terminate();
    lua_close(L);
    return 0;
}