#include "Fuse.h"

// ������
Fuse::Fuse() : status(AVAILABLE) {}
Fuse::Fuse(int id, int itemType) : id(id), status(AVAILABLE) {}