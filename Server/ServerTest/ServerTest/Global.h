#pragma once
#include "SESSION.h"
#include "stdafx.h"
#include <mutex>

extern array<SESSION, MAX_USER> clients;
extern array<SESSION, MAX_ITEM> items;
 