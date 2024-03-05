#pragma once
#include "utilities_elev.h"

void ChangePos(int posArray[], const int *pos_index);
int getPosIndex(const int posArray[]);
void UpdatePosArray(int posArray[], int *floor_sensor, state *elev_state);
void printPosArray(int posArray[]);
void DeleteOrderWithSensor(char orderArray[]);
void AddOrders(char orderArray[]);
void UpdateCurrentFloorInOrders(const int orderArray[], const int *current_floor, bool *current_floor_in_orders);
