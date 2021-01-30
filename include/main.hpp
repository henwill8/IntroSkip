#pragma once

#include <unordered_set>
#include <map>
#include <list>
#include <tuple> // for tuple 
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"  
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "extern/questui/shared/BeatSaberUI.hpp"
#include "extern/custom-types/shared/macros.hpp"
#include "extern/custom-types/shared/register.hpp"
#include "extern/custom-types/shared/types.hpp"
#include "../extern/beatsaber-hook/shared/utils/utils.h"  
#include "../extern/beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"
#include "../extern/modloader/shared/modloader.hpp"

using namespace il2cpp_utils;

Configuration& getConfig();

Logger& logger();
