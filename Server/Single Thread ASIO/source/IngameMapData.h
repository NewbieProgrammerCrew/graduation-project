#pragma once

#include "FuseBox.h"
#include "Portal.h"
#include "Fuse.h"

class IngameMapData
{
public:
	int					_map_num;
	int					_fuse_box_list[8];
	array <FuseBox, 8>	_fuse_boxes;						// ǻ�� �ڽ� ��ġ ����
	Portal				_portal;
	int					_player_ids[5];
	array<Fuse, 8>		_fuses;
	

	IngameMapData() {
		for (int i = 0; i < 5; ++i)
			_player_ids[i] = -1;
	};
};

