// Alec Hoffmann
// InitAIC23.h
// Lab 4
// Header for codec initialization functions.

#ifndef INITAIC23_H_
#define INITAIC23_H_

#include <DSP28x_Project.h>

void InitMcBSPb();
void InitSPIA();
void InitAIC23();
void CheckCodecSpi();
void DisableAnalogBypass();

#endif /* INITAIC23_H_ */
