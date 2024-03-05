#pragma once
#include "utilities_elev.h"

void ChangePos(int posArray[], const int *pos_index);
int getPosIndex(const int posArray[]);
void UpdatePosArray(int posArray[], const state *elev_state);
void DeleteOrderWithSensor(char orderArray[]);
void AddOrders(char orderArray[]);
void UpdateCurrentFloorInOrders(const char orderArray[], const int *current_floor, bool *current_floor_in_orders);
void UpdateMinAndMaxOrder(const char orderArray[], int *minOrder, int *maxOrder);
void UpdateOrdersEmpty(const char orderArray[], bool *orders_empty);

void printPosArray(const int posArray[]);