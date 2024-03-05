#include "lightArray.h"

void ActuateDirAndCabLight(const char orderArray[]) {
    for (int i=0; i<N_FLOORS; i++) {
        char order = orderArray[i];
        if (order == 'N') {
            elevio_buttonLamp(i,BUTTON_CAB,0);
        }
        else {
            elevio_buttonLamp(i,BUTTON_CAB,1);
        }
        
        if (order == 'U') {
            elevio_buttonLamp(i, BUTTON_HALL_UP, 1);
            elevio_buttonLamp(i, BUTTON_HALL_DOWN, 0);
        }
        else if (order == 'D') {
            elevio_buttonLamp(i, BUTTON_HALL_DOWN, 1);
            elevio_buttonLamp(i,BUTTON_HALL_UP, 0);
        }
        else {
            elevio_buttonLamp(i, BUTTON_HALL_DOWN, 0);
            elevio_buttonLamp(i, BUTTON_HALL_UP, 0);
        }
    }
}

void UpdateFloorIndicator(void) {
    int floorIndeks = elevio_floorSensor();
    if (floorIndeks != -1) {
        elevio_floorIndicator(floorIndeks);
    }
}


