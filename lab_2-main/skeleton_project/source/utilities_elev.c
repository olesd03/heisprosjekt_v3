#include "utilities_elev.h"
#include "driver/elevio.h"
#include <time.h>
#include "orderArray.h"
#include "lightArray.h"
#include <stdio.h>


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
    double Time = 3.0;
    elevio_motorDirection(DIRN_STOP);
    elevio_doorOpenLamp(1);
    DeleteOrderWithSensor(orderArray);
    ActuateDirAndCabLight(orderArray);
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

        //Printer tilstander
        printf("{");
        for (int i = 0; i<(N_FLOORS-1); i++) {
            printf("%c,",orderArray[i]);
        }
        printf("%c}\n",orderArray[N_FLOORS-1]);
        printf("current_floor: %d\n", *current_floor);
        printf("elev_state: %d\n", *elev_state);
        printf("current_floor_in_orders: %d\n\n\n",*current_floor_in_orders);




        nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
        AddOrders(orderArray); //Add later
        UpdateCurrentFloorInOrders(orderArray, current_floor, current_floor_in_orders);
        UpdateCurrentFloor(current_floor);
        if (*current_floor_in_orders) {
            DeleteOrderWithSensor(orderArray);
            Time = 3.0;
        }
    }
    elevio_doorOpenLamp(0);
    return false;
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

void ChangeStateBetween(int posArray[], char orderArray[], state *elev_state) {
    int pos_index = getPosIndex(posArray);
    int imm_array[2*N_FLOORS-1] = {0};
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

void UpdateAndPrintStates(const int posArray[],const char orderArray[],int *minOrder, int *maxOrder, int *current_floor,
                         bool *orders_empty, bool *floor_stop, bool *current_floor_in_orders, state *elev_state) {
    UpdateCurrentFloor(current_floor);
    UpdateOrdersEmpty(orderArray, orders_empty);
    UpdateFloorStop(orderArray, current_floor, floor_stop, elev_state);
    UpdateCurrentFloorInOrders(orderArray, current_floor, current_floor_in_orders);

    printf("{");
    for (int i = 0; i<(2*N_FLOORS-2); i++) {
        printf("%d,",posArray[i]);
    }
    printf("%d}\n",posArray[2*N_FLOORS-2]);

    printf("{");
    for (int i = 0; i<(N_FLOORS-1); i++) {
        printf("%c,",orderArray[i]);
    }
    printf("%c}\n",orderArray[N_FLOORS-1]);

    printf("Min- og MaxOrder: %d, %d\n", *minOrder, *maxOrder);
    printf("orders_empty: %d\n", *orders_empty);
    printf("floor_stop: %d\n", *floor_stop);
    printf("current_floor_in_orders: %d\n", *current_floor_in_orders);
    printf("current_floor: %d\n", *current_floor);
    if (*elev_state == -1) {printf("elev_state: down\n");}
    if (*elev_state == 0) {printf("elev_state: up\n");}
    if (*elev_state == 1) {printf("elev_state: idle\n");}
    if (*elev_state == 2) {printf("elev_state: emstop\n");}
    
    printf("floor_sensor = %d\n\n\n",elevio_floorSensor());
    

}

/*
typedef enum {
    down = -1,
    up = 0,
    idle = 1,
    emstop = 2
} state;
*/










/*
state elev_state = idle;
bool floor_stop = false;
bool orders_empty;
bool current_floor_in_orders;
int current_floor;
int maxOrder = 0;
int minOrder = 0;
char orderArray[N_FLOORS];
for (int i = 0; i<N_FLOORS; i++) {orderArray[i]='N';}
int posArray[2*N_FLOORS-1] = {0};
*/