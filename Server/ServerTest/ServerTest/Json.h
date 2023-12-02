#include "types.h"

class objects {
public:
	bool	in_use;
	int		type;
	string	obj_name;
	float	pos_x;
	float	pos_y;
	float	pos_z;
	float	extent_x;
	float	extent_y;
	float	extent_z;
	float	yaw;
	float	roll;
	float	pitch;

	objects() {
		in_use = false;
		type = NULL;
	}
	~objects() {
		in_use = false;
	};
};

extern map<int,objects> ST1_OBJ;
extern map<int,objects> ST2_OBJ;
extern map<int,objects> ST3_OBJ;



"BP_Jelly_C_0_955AE44741817DFD7815F9B7BF37C6AD": [
{
	"Type": "Box",
		"LocationX" : 1270.0370060111857,
		"LocationY" : -2.9612410164232776,
		"LocationZ" : 91.148533319730404,
		"ExtentX" : 93.120000064373016,
		"ExtentY" : 96,
		"ExtentZ" : 92.160000085830688,
		"Yaw" : 0,
		"Roll" : -0,
		"Pitch" : 0
}