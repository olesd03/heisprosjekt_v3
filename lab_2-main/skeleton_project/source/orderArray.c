#include "orderArray.h"
#include "driver/elevio.h"

void ChangePos(int posArray[], const int *pos_index) {
    int length = 2*N_FLOORS-1;
    for (int i=0; i<length; i++) {
        if (i ==  *pos_index) {
            posArray[i] = 1;
        }
        else {
            posArray[i] = 0;
        }
    }
}

int getPosIndex(const int posArray[]) {
    int length = 2*N_FLOORS-1;
    for (int i = 0; i<length; i++) {
        if (posArray[i] != 0) {
            return i;
            break;
        }
    }
}

void UpdatePosArray(int posArray[], int *floor_sensor, state *elev_state) {
    int pos_index;
    if (pos_index == (2*N_FLOORS-1)) {
            pos_index = 2*(*floor_sensor);
            ChangePos(posArray, &pos_index);
    }
    else {
        pos_index = getPosIndex(posArray);
    }


    if (*floor_sensor == -1) {
        if (*elev_state == up) {
            if ((pos_index % 2) == 0) {
                pos_index ++;
                ChangePos(posArray, &pos_index);
            }
        }
        else {
            if (*elev_state == down) {
                if ((pos_index % 2) == 0) {
                    pos_index --;
                    ChangePos(posArray, &pos_index);
                }
            }
        }
    }
    else {
        if (*elev_state == up) {
            if ((pos_index % 2) != 0) {
                pos_index ++;
                ChangePos(posArray, &pos_index);
            }
        }
        else {
            if (*elev_state == down) {
                if ((pos_index % 2) != 0) {
                    pos_index --;
                    ChangePos(posArray, &pos_index);
                }
            }
        }
    }
}

void printPosArray(int posArray[]) {
    printf("[%d,%d,%d,%d,%d,%d,%d]\n",posArray[0],posArray[1],posArray[2],posArray[3],posArray[4],posArray[5],posArray[6]);
}