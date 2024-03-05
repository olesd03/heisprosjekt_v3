#pragma once
#include "driver/elevio.h"
#include <time.h>
#include "orderArray.h"

typedef enum {
    down = -1,
    up = 0,
    idle = 1,
    emstop = 2
} state;

typedef enum {
    false = 0,
    true = 1
} bool;


void GoUpToClosest(void);
bool EmstopInit(state *elev_state);
void UpdateCurrentFloor(int *current_floor);
bool StopAndLight(char orderArray[], int *current_floor, state *elev_state, bool *current_floor_in_orders);
void UpdateFloorStop(const char orderArray[], const int *current_floor, bool *floor_stop, const state *elev_state);
void changeStateBetween(int posArray[], char orderArray[], state *elev_state);