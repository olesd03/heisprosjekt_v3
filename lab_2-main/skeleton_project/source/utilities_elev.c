#include "utilities_elev.h"
#include "driver/elevio.h"

void GoUpToClosest(void) {
    while (elevio_floorSensor() == -1) {
        elevio_motorDirection(DIRN_UP); 
    }
}

bool BreakAndEmstop(state *elev_state) {
    if (elevio_stopButton()) {
        *elev_state = emstop;
        return true;
    }
    else {
        return false;
    }
}

void UpdateCurrentFloor(int *current_floor) {
    if (elevio_floorSensor() != -1) {
        *current_floor = elevio_floorSensor() +1;
    }
}