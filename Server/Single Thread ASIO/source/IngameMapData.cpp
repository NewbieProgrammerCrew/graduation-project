#include "IngameMapData.h"

int IngameMapData::GetRealFuseBoxIndex(int index)
{
    for (int i : fuse_box_list_) {
        if (i == index)
            return i;
    }
}
