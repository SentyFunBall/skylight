#include "funcs.hpp"

#include <string>
#include <iostream>
#include <chrono>

#include "state.h"

#include "miniscl.hpp"

namespace Lfuncs {

int require(lua_State* L) {
    // Only want to load Lua files
    std::string moduleName = luaL_checkstring(L, 1);
    if (moduleName.empty()) {
        lua_pushnil(L);
        return 1;
    }

    // Append ".lua" to the module name
    std::string fullModuleName = scriptDir + "/" + moduleName + ".lua";
    std::cout << "Requiring module: " << fullModuleName << std::endl;

    // Load the module
    if (luaL_loadfile(L, fullModuleName.c_str()) != LUA_OK) {
        lua_pushnil(L);
        return 1;
    }

    // Execute the loaded module
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        lua_pushnil(L);
        return 1;
    }

    // The module's return value is now on the stack, return it
    return 1;
}

int tlength(lua_State* L) {
    if (!lua_istable(L, 1)) {
        lua_pushinteger(L, 0); // Return 0 for non-table inputs
        return 1;
    }
    int length = lua_rawlen(L, 1);
    lua_pushinteger(L, length);
    return 1; // One return value (the length)
}

} // namespace funcs

namespace Skylight {

int Lua::LFuncs::gettime(lua_State* L) {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    lua_pushnumber(L, elapsed);
    return 1; // One return value (the time)
}

int Lua::LFuncs::getConfigProperty(lua_State* L) {
    std::string key = luaL_checkstring(L, 1);
    for (const auto& prop : configProperties) {
        if (prop.first == key) {
            lua_pushstring(L, prop.second.c_str());
            return 1; // Found the property, return its value
        }
    }
    lua_pushnil(L); // Property not found
    return 0;
}

int Lua::LFuncs::setConfigProperty(lua_State* L) {
    // Expecting two arguments: key (string) and value (string, number, or boolean)
    scl::string key = luaL_checkstring(L, 1);
    scl::string value = "";
    if (luaL_checkstring(L, 2)) {
        value = luaL_checkstring(L, 2);
    } else if (lua_isboolean(L, 2)) {
        value = lua_toboolean(L, 2) ? "true" : "false";
    } else if (lua_isnumber(L, 2)) {
        value = std::to_string(lua_tonumber(L, 2)).c_str(); // this is ridiculous
    } else {
        lua_pushboolean(L, false);
        return 1;
    }

    bool foundExisting = false;

    // Load the XML config file
    scl::xml::XmlDocument doc;
    std::string configPath = scriptDir + "/config.xml";
    if (doc.load_file(configPath.c_str()).code != scl::xml::OK) {
        lua_pushboolean(L, false);
        return 1;
    }
    std::vector<scl::xml::XmlElem*> writeable = doc.find_children("writeable");
    if (writeable.empty()) {
        lua_pushboolean(L, false);
        return 1;
    }

    // Search for and update existing setting if it exists
    std::vector<scl::xml::XmlElem*> settings = writeable[0]->find_children("setting");
    for (scl::xml::XmlElem* setting : settings) {
        scl::xml::XmlAttr* attr = setting->find_attr("key");
        if (attr && attr->data() == key) {
            scl::xml::XmlAttr* valueAttr = setting->find_attr("value");
            if (valueAttr) {
                valueAttr->set_data(doc, value);
            } else {
                // Key exists but no value attribute??? Create it
                valueAttr = new scl::xml::XmlAttr();
                valueAttr->set_tag(doc, "value");
                valueAttr->set_data(doc, value);
                setting->add_attr(valueAttr);
            }
            foundExisting = true;
            break;
        }
    }

    if (!foundExisting) {
        // Key not found, add new setting
        scl::xml::XmlElem* elem = new scl::xml::XmlElem();
        elem->set_tag(doc, "setting");
        scl::xml::XmlAttr* keyAttr = new scl::xml::XmlAttr();
        keyAttr->set_tag(doc, "key");
        keyAttr->set_data(doc, key);
        elem->add_attr(keyAttr);

        scl::xml::XmlAttr* valueAttr = new scl::xml::XmlAttr();
        valueAttr->set_tag(doc, "value");
        valueAttr->set_data(doc, value);
        elem->add_attr(valueAttr);

        writeable[0]->add_child(elem);
    }

    // Write to disk 
    scl::stream file;
    file.open(configPath.c_str(), "w");

    doc.print(file);
    file.flush();
    file.close();

    // Update in-memory config as well
    for (auto& prop : configProperties) {
        if (prop.first == key.cstr()) {
            prop.second = value.cstr();
            break;
        }
    }

    lua_pushboolean(L, 1); // Success
    return 1;
}

} // namespace Skylight