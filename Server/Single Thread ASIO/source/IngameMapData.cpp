#include "IngameMapData.h"

int IngameMapData::GetRealFuseBoxIndex(int index)
{
    for (int i : _fuse_box_list) {
        if (i == index)
            return i;
    }
}
