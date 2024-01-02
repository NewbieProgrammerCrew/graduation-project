#pragma once

enum FuseStatus { AVAILABLE, ACQUIRED };

class Fuse
{
    int id;
    FuseStatus status;
public:
    Fuse();
    Fuse(int id, int itemType);
    int GetStatus() { return status; }
    void SetStatus(FuseStatus st) { status = st; }
};