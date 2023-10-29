#pragma once

enum ItemStatus { AVAILABLE, ACQUIRED };

class Item
{
    int id;
    int itemType;
    ItemStatus status;
public:
  
    Item();
    Item(int id, int itemType);
    int GetStatus() { return status; }
    void SetStatus(ItemStatus st) { status = st; }


};

