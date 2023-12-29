#pragma once
class FuseBox
{
public:
	int index;
	int color;
	bool active;
	int matchIndex;
	

	FuseBox() {
		index = -1;
		color = -1;
		active = false;
		matchIndex = -1;
	}
	~FuseBox() {}
};

