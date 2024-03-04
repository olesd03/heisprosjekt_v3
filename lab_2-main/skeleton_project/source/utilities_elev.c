#include "utilities_elev.h"
#include "driver/elevio.h"
#include <time.h>

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

bool StopAndLight(char orderArray[], int *current_floor, state *elev_state, bool *current_floor_in_orders) {
    double Time = 3;
    elevio_doorOpenLamp(1);
    while(Time >= 0) {
        if (elevio_stopButton()) {
            elev_state = emstop;
            return true;
        }

        if (elevio_obstruction()) {
            Time = 3.0;
        }
        else {
            Time -= 0.010;
        }

        nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
        AddOrders(orderArray); //Add later
        UpdateCurrentFloorInOrders(&current_floor_in_orders); //Add later
        UpdateCurrentFloor(current_floor);
        if (*current_floor_in_orders) {
            DeleteOrder(orderArray, current_floor);
            Time = 3.0;
        }
    }
    elevio_doorOpenLamp(0);
}