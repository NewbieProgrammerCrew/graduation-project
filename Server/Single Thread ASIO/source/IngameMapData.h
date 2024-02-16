#pragma once

#include "FuseBox.h"
#include "Portal.h"

class IngameMapData
{
public:
	int					_fuse_box_list[8];
	array <FuseBox, 8>	_fuse_boxes;						// 퓨즈 박스 위치 정보
	Portal				_portal;
	int					_player_ids[5];

	IngameMapData() {
		for (int i = 0; i < 5; ++i)
			_player_ids[i] = -1;
	};
};

