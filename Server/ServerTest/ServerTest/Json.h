#include "types.h"

class Object {
public:
	bool	in_use;
    int     map_num;
	int		type;
	string	obj_name ;
	float	pos_x;
	float	pos_y;
	float	pos_z;
	float	extent_x;
	float	extent_y;
	float	extent_z;
	float	yaw;
	float	roll;
	float	pitch;


	Object() {
		in_use = false;
		type = NULL;
        obj_name = "";
	}
	~Object() {
		in_use = false;
	};
};


struct ObjectData {
    double locationX, locationY, locationZ;
    double extentX, extentY, extentZ;
    double yaw, pitch, roll;

    void readFromJSON(const rapidjson::Value& value) {
        if (value.HasMember("LocationX") && value["LocationX"].IsNumber())
            locationX = value["LocationX"].GetDouble();
        if (value.HasMember("LocationY") && value["LocationY"].IsNumber())
            locationY = value["LocationY"].GetDouble();
        if (value.HasMember("LocationZ") && value["LocationZ"].IsNumber())
            locationZ = value["LocationZ"].GetDouble();
        if (value.HasMember("ExtentX") && value["ExtentX"].IsNumber())
            extentX = value["ExtentX"].GetDouble();
        if (value.HasMember("ExtentY") && value["ExtentY"].IsNumber())
            extentY = value["ExtentY"].GetDouble();
        if (value.HasMember("ExtentZ") && value["ExtentZ"].IsNumber())
            extentZ = value["ExtentZ"].GetDouble();
        if (value.HasMember("Yaw") && value["Yaw"].IsNumber())
            yaw = value["Yaw"].GetDouble();
        if (value.HasMember("Pitch") && value["Pitch"].IsNumber())
            pitch = value["Pitch"].GetDouble();
        if (value.HasMember("Roll") && value["Roll"].IsNumber())
            roll = value["Roll"].GetDouble();
    }
};



//
//"BP_Jelly_C_0_955AE44741817DFD7815F9B7BF37C6AD": [
//{
//	"Type": "Box",
//		"LocationX" : 1270.0370060111857,
//		"LocationY" : -2.9612410164232776,
//		"LocationZ" : 91.148533319730404,
//		"ExtentX" : 93.120000064373016,
//		"ExtentY" : 96,
//		"ExtentZ" : 92.160000085830688,
//		"Yaw" : 0,
//		"Roll" : -0,
//		"Pitch" : 0
//}