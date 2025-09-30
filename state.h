#pragma once

#include <cstddef>
#include <string>
#include <chrono>

#include "lua.hpp"

extern std::string scriptDir;
extern std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
extern std::vector<std::pair<std::string, std::string>> configProperties;
extern lua_State*                                       L;
extern int                                              tickRef;
extern int                                              initRef;
