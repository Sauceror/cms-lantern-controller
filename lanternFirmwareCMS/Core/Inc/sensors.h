#ifndef SENSORS_H
#define SENSORS_H
#include <stdint.h>

void Sensors_Init(void);
void Sensors_Task(void);   /* Updates Modbus input regs */

#endif
