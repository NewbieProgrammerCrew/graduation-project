#include "IngameMapData.h"

int IngameMapData::GetRealFuseBoxIndex(int index)
{
	for (int i = 0; i < fuse_box_list_.size(); ++i ) {
		if (fuse_box_list_[i] == index)
			return i;
	}
	return -1;
}