#include "Fuse.h"

// ������
Fuse::Fuse() : status_(AVAILABLE) {}
Fuse::Fuse(int id, int itemType) : id_(id), status_(AVAILABLE) {}