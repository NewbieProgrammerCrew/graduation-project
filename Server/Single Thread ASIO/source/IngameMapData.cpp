#include "IngameMapData.h"

int IngameMapData::GetRealFuseBoxIndex(int index)
{
	for (int i = 0; i < fuse_boxes_.size(); ++i ) {
		if (fuse_box_list_[i] == index)
			return i;
	}
}

int IngameMapData::GetFakeFuseBoxIndex(int index)
{
	return fuse_box_list_[index];
}
