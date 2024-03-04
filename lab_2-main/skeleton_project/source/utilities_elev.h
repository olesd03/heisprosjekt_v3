#pragma once

typedef enum {
    down=-1,
    up=0,
    idle=1,
    emstop=2
} state;

typedef enum {
    false = 0,
    true = 1
} bool;


void GoUpToClosest(void);
bool BreakAndEmstop(state *elev_state);
void UpdateCurrentFloor(int *current_floor);