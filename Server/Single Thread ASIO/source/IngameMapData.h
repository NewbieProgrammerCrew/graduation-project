#pragma once

#include "FuseBox.h"
#include "Portal.h"
#include "Fuse.h"
#include "ItemBox.h"

class IngameMapData
{
public:
	int					map_num_;
	array<int, 8>		fuse_box_list_;		// Real Fuse Boxes Index;		
	array <FuseBox, 8>	fuse_boxes_;		
	array<ItemBox, 10>	ItemBoxes_;
	Portal				portal_;
	int					player_ids_[5];
	array<Fuse, 8>		fuses_;
	array<int, MAX_JELLY_NUM>	jellies{ 0 };
	int					in_game_users_num_;
	vector<int>			finished_player_list_;


	IngameMapData() {
		for (int i = 0; i < 5; ++i){
			player_ids_[i] = -1;
		}
		map_num_ = -1;
		in_game_users_num_ = 0;
	};
	int GetRealFuseBoxIndex(int index);
};

 