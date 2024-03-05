#include "utilities_elev.h"


void GoUpToClosest(void) {
    while (elevio_floorSensor() == -1) {
        elevio_motorDirection(DIRN_UP); 
    }
}

bool EmstopInit(state *elev_state) {
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
            *elev_state = emstop;
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
        UpdateCurrentFloorInOrders(orderArray, current_floor, current_floor_in_orders);
        UpdateCurrentFloor(&current_floor);
        if (*current_floor_in_orders) {
            DeleteOrderWithSensor(orderArray);
            Time = 3.0;
        }
    }
    DeleteOrderWithSensor(orderArray);
    elevio_doorOpenLamp(0);

}

void UpdateFloorStop(const char orderArray[], const int *current_floor, bool *floor_stop, const state *elev_state) {
    *floor_stop = false;
    char order = orderArray[( *current_floor - 1)];
    if (order != 'N') {
        if (order != 'C') {
            if (order != 'D') {
                if (*elev_state == up) {
                    *floor_stop = true;
                }
            }
            else {
                if (*elev_state == down) {
                    *floor_stop = true;
                }
            }
        }
        else {
            *floor_stop = true;
        }
    }
}

void changeStateBetween(int posArray[], char orderArray[], state *elev_state) {
    int pos_index = getPosIndex(posArray);
    int imm_array[N_FLOORS] = {0};
    for (int i = 0; i<N_FLOORS; i++) {
        if (orderArray[i] != 'N') {
            imm_array[2*i] = 1; 
        }
    }
    for (int i = 0; i<(2*N_FLOORS - 1); i += 2) {
        if (imm_array[i] == 1) {
            if (pos_index > i) {
                *elev_state = down;
                break;
            }
            else if (pos_index < i) {
                *elev_state = up;
                break;
            }
        }
    }
}