#pragma once

#include "FuseBox.h"
#include "Portal.h"
#include "Fuse.h"
#include "ItemBox.h"

class IngameMapData
{
public:
	int					_map_num;
	int					_fuse_box_list[8];					// ??
	array <FuseBox, 8>	_fuse_boxes;						// 퓨즈 박스 위치 정보
	array<ItemBox, 10>	_ItemBoxes;
	Portal				_portal;
	int					_player_ids[5];
	array<Fuse, 8>		_fuses;


	IngameMapData() {
		for (int i = 0; i < 5; ++i)
			_player_ids[i] = -1;
	};
	int GetRealFuseBoxIndex(int index);
};

