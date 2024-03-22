#pragma once

#include "FuseBox.h"
#include "Portal.h"
#include "Fuse.h"
#include "ItemBox.h"

class IngameMapData
{
public:
	int					map_num_;
	int					fuse_box_list_[8];					// ??
	array <FuseBox, 8>	fuse_boxes_;						// ǻ�� �ڽ� ��ġ ����
	array<ItemBox, 10>	ItemBoxes_;
	Portal				portal_;
	int					player_ids_[5];
	array<Fuse, 8>		fuses_;


	IngameMapData() {
		for (int i = 0; i < 5; ++i)
			player_ids_[i] = -1;
	};
	int GetRealFuseBoxIndex(int index);
};

