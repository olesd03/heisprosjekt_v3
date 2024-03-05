#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "orderArray.h"
#include "utilities_elev.h"
#include "lightArray.h"





int main(){
    elevio_init();
    goUpToClosest();

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


    while(1){
        while(elev_state == down) {
            elevio_motorDirection(DIRN_DOWN);
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            if (EmstopInit(&elev_state)) {break;}
            UpdateCurrentFloor(&current_floor);
            UpdateCurrentFloorInOrders(orderArray, &current_floor, &current_floor_in_orders);
            AddOrders(orderArray);
            ActuateDirAndCabLight(orderArray);
            UpdatePosArray(posArray, &elev_state);

            if (elevio_floorSensor() != -1) {
                UpdatePosArray(posArray, &elev_state);
                if (current_floor == minOrder) {
                    if (StopAndLight(orderArray, &current_floor, &elev_state, &current_floor_in_orders)) {break;} 
                    UpdateMinAndMaxOrder(orderArray, &minOrder, &maxOrder); 
                    if ((current_floor > minOrder) && (minOrder != 0)) {
                        elev_state = down;
                    }
                    else {
                        if ((current_floor < maxOrder) && (maxOrder != 0)) {
                            elev_state = up;
                        }
                        else {
                            elev_state = idle;
                        }
                    }
                }
                else {
                    UpdateFloorStop(orderArray, &current_floor, &floor_stop, &elev_state); 
                    if (floor_stop) {
                        if (StopAndLight(orderArray, &current_floor, &elev_state, &current_floor_in_orders)) {break;}
                    }
                }
            }
            UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
        }
        while(elev_state == up) {
            elevio_motorDirection(DIRN_UP);
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            UpdateCurrentFloor(&current_floor);
            UpdateCurrentFloorInOrders(orderArray, &current_floor, &current_floor_in_orders);
            AddOrders(orderArray);
            ActuateDirAndCabLight(orderArray);
            UpdatePosArray(posArray, &elev_state);
            if (EmstopInit(&elev_state)) {break;}
            

            if (elevio_floorSensor() != -1) {
                UpdatePosArray(posArray, &elev_state);
                if (current_floor == maxOrder) {
                    if (StopAndLight(orderArray, &current_floor, &elev_state, &current_floor_in_orders)) {break;} 
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
                    UpdateFloorStop(orderArray, &current_floor, &floor_stop, &elev_state);
                    if (floor_stop) {
                        if (StopAndLight(orderArray, &current_floor, &elev_state, &current_floor_in_orders)) {break;} 
                    }
                }
            }
            UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
        }
        while(elev_state == idle) {
            GoUpToClosest();
            nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
            AddOrders(orderArray);
            ActuateDirAndCabLight(orderArray);
            if (EmstopInit(&elev_state)) {break;}
            UpdateOrdersEmpty(orderArray, &orders_empty); 
            if (!orders_empty) {
                UpdateCurrentFloor(&current_floor);
                UpdateCurrentFloorInOrders(orderArray, &current_floor, &current_floor_in_orders);
                if (current_floor_in_orders) {
                    if (StopAndLight(orderArray, &current_floor, &elev_state, &current_floor_in_orders)) {break;}
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
            UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
        }




        
        while(elev_state == emstop) {
            bool breakOuter = false;
            DeleteAllOrders(orderArray);
            ActuateDirAndCabLight(orderArray);
            elevio_motorDirection(DIRN_STOP);    
            UpdateOrdersEmpty(orderArray, &orders_empty); 
            elevio_stopLamp(1);
            

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

                    UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                    if (EmstopInit(&elev_state)) {breakOuter=true;  break;}
                    else {
                        elevio_stopLamp(0);
                        AddOrders(orderArray);
                        ActuateDirAndCabLight(orderArray); 
                    }
                }
                if (breakOuter) {break;}
                elevio_doorOpenLamp(false);
                

                UpdateOrdersEmpty(orderArray, &orders_empty);  
                while (orders_empty) {
                    UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
                    AddOrders(orderArray);          
                    UpdateOrdersEmpty(orderArray, &orders_empty);  
                    if (EmstopInit(&elev_state)) {breakOuter=true; break;}
                    elevio_stopLamp(0);
                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                }
                if (breakOuter) {break;}
                ActuateDirAndCabLight(orderArray);

                UpdateCurrentFloorInOrders(orderArray, &current_floor, &current_floor_in_orders); 
                UpdateMinAndMaxOrder(orderArray, &minOrder, &maxOrder);
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
                UpdateOrdersEmpty(orderArray, &orders_empty);
                while (orders_empty) {
                    UpdateAndPrintStates(posArray, orderArray, &minOrder, &maxOrder, &current_floor, &orders_empty, &floor_stop, &current_floor_in_orders, &elev_state);
                    nanosleep(&(struct timespec){0, 10*1000*1000}, NULL);
                    if (EmstopInit(&elev_state)) {breakOuter=true; break;}
                    elevio_stopLamp(0);
                    AddOrders(orderArray); 
                    UpdateOrdersEmpty(orderArray, &orders_empty); 
                }
                if (breakOuter) {break;}
                ActuateDirAndCabLight(orderArray);
                ChangeStateBetween(posArray, orderArray, &elev_state);
            }
        }
    }

    return 0;
}
