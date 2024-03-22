#pragma once

enum FuseStatus { AVAILABLE, ACQUIRED };

class Fuse
{
public:
    int id_;
    FuseStatus status_;
public:
    Fuse();
    Fuse(int id, int itemType);
};