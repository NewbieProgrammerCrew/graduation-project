#include "Item.h"
// »ý¼ºÀÚ
Item::Item() : status(AVAILABLE) {}
Item::Item(int id, int itemType) : id(id), itemType(itemType), status(AVAILABLE) {}