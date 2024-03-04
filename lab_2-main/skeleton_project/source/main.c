#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

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


int main(){
    elevio_init();
    goUpToClosest();

    state elev_state = idle;
    bool floor_stop;
    bool orders_empty;
    bool current_floor_in_orders;
    int current_floor;
    int maxOrder = 0;
    int minOrder = 0;
    char orderArray[N_FLOORS];
    for (int i = 0; i<N_FLOORS; i++) {orderArray[i]='N';}
    int posArray[2*N_FLOORS-1] = {0};


    while(1){
        while(elev_state == down) {
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            if (breakAndEmstop(&elev_state)) {break;}
            elevio_motorDirection(DIRN_DOWN);
            UpdateCurrentFloor(&current_floor);
            AddOrders(orderArray);
            

            if (elevio_floorSensor() != -1) {
                if (current_floor == minOrder) {
                    StopAndLight();  // Add later
                    UpdateMinAndMaxOrder(orderArray, &minOrder, &maxOrder); // Add later
                    if (current_floor > minOrder) {
                        elev_state = down;
                    }
                    else {
                        if (current_floor < maxOrder) {
                            elev_state = up;
                        }
                        else {
                            elev_state = idle;
                        }
                    }
                }
                else {
                    UpdateFloorStop(orderArray, &current_floor); // Add later
                    if (floor_stop) {
                        StopAndLight();
                    }
                }
            }
        }
        while(elev_state == up) {
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            if (breakAndEmstop(&elev_state)) {break;}
            elevio_motorDirection(DIRN_UP);
            UpdateCurrentFloor(&current_floor);
            AddOrders(orderArray);
            

            if (elevio_floorSensor() != -1) {
                if (current_floor == maxOrder) {
                    StopAndLight();
                    UpdateMinAndMaxOrder(orderArray, &minOrder, &maxOrder);
                    if (current_floor < maxOrder) {
                        elev_state = up;
                    }
                    else {
                        if (current_floor > minOrder) {
                            elev_state = down;
                        }
                        else {
                            elev_state = idle;
                        }
                    }
                }
                else {
                    UpdateFloorStop(orderArray, &orderArray);
                    if (floor_stop) {
                        StopAndLight();
                    }
                }
            }
        }
        while(elev_state == idle) {
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            if (breakAndEmstop(&elev_state)) {break;}
            GoUpToClosest();
            UpdateOrdersEmpty(); //Add later
            if (!orders_empty) { 
                UpdateCurrentInOrders(); //Add later
                if (current_floor_in_orders) {
                    StopAndLight();
                }
                else {
                    if (current_floor > minOrder) {
                        elev_state = down;
                    }
                    else {
                        elev_state = up;
                    }
                }
            }
        }




        
        while(elev_state == emstop) {
            bool breakOuter = false;
            DeleteOrders();     // Add later
            UpdateOrdersEmpty(); // Add later
            elevio_motorDirection(DIRN_STOP);

            if (elevio_floorSensor() != -1) {
                UpdateCurrentFloor(&current_floor);

                elevio_doorOpenLamp(true);
                double Time = 3;
                while (Time >= 0) {
                    if (elevio_obstruction()) {
                        Time = 3;
                    }
                    else {
                        Time -= 0.01;
                    }

                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                    if (breakAndEmstop(&elev_state)) {
                        breakOuter=true;
                        break;
                    }
                    else {
                        AddOrders(); // Add later
                    }
                }
                if (breakOuter) {break;}
                elevio_doorOpenLamp(false);
                

                UpdateOrdersEmpty();  // Add later
                while (orders_empty) {
                    AddOrders();          // Add later
                    UpdateOrdersEmpty();  // Add later
                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                    if (breakAndEmstop(&elev_state)) {
                        breakOuter=true;
                        break;
                    }
                }
                if (breakOuter) {break;}


                UpdateCurrentFloorInOrders(); // Add later
                UpdateMinAndMaxOrder();
                if (current_floor_in_orders) {
                    elev_state = idle;
                }   
                else {
                    if (current_floor < maxOrder) {
                        elev_state = up;
                    }
                    else {
                        elev_state = down;
                    }
                }
            }
            else {
                UpdateOrdersEmpty(orderArray);
                while (orders_empty) {
                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                    if (breakAndEmstop(&elev_state)) {
                        breakOuter=true;
                        break;
                    }
                    AddOrders(orderArray); // Add later
                    UpdateOrdersEmpty(orderArray, &orders_empty); // Add later
                }
                if (breakOuter) {break;}
                ChangeStateBetween(&elev_state); // Add later
                
            }
        }
    }

    return 0;
}
