#include "Fuse.h"

// »ý¼ºÀÚ
Fuse::Fuse() : status_(AVAILABLE) {}
Fuse::Fuse(int id, int itemType) : id_(id), status_(AVAILABLE) {}