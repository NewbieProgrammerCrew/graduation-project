#define COL_SECTOR_SIZE 800
#define MAP_X 12600
#define MAP_Y 12600
#define MAP_Z 7300
#define SEC_TO_MICRO 1000000
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>
#include <rapidjson/istreamwrapper.h>
#include <cmath>
#include <algorithm>
#include <numbers>
#include <chrono>


#include "rapidjson/document.h"
#include "SESSION.h"
#include "stdafx.h"
#include "OVER_EXP.h"
#include "protocol.h"

using namespace rapidjson;
using namespace std;