/*
 * stepper_ctrl.h
 *
 *  Created on: May 7, 2021
 *      Author: YAN
 */

#ifndef INC_STEPPER_CTRL_H_
#define INC_STEPPER_CTRL_H_

#include "main.h"

#define HIGH 1
#define LOW  0

#define ON  HIGH
#define OFF LOW

#define CW  HIGH
#define CCW LOW

#define MOTOR_X_EN(x)         x ? LL_GPIO_SetOutputPin(MOTOR_X_EN_GPIO_Port,MOTOR_X_EN_Pin) : LL_GPIO_ResetOutputPin(MOTOR_X_EN_GPIO_Port,MOTOR_X_EN_Pin)
#define MOTOR_X_PUL(x)        x ? LL_GPIO_SetOutputPin(MOTOR_X_PUL_GPIO_Port, MOTOR_X_PUL_Pin) : LL_GPIO_ResetOutputPin(MOTOR_X_PUL_GPIO_Port, MOTOR_X_PUL_Pin)
#define MOTOR_X_DIR(x)        x ? LL_GPIO_SetOutputPin(MOTOR_X_DIR_GPIO_Port, MOTOR_X_DIR_Pin) : LL_GPIO_ResetOutputPin(MOTOR_X_DIR_GPIO_Port, MOTOR_X_DIR_Pin)

#define MOTOR_Y_EN(x)         x ? LL_GPIO_SetOutputPin(MOTOR_Y_EN_GPIO_Port,MOTOR_Y_EN_Pin) : LL_GPIO_ResetOutputPin(MOTOR_Y_EN_GPIO_Port,MOTOR_Y_EN_Pin)
#define MOTOR_Y_PUL(x)        x ? LL_GPIO_SetOutputPin(MOTOR_Y_PUL_GPIO_Port, MOTOR_Y_PUL_Pin) : LL_GPIO_ResetOutputPin(MOTOR_Y_PUL_GPIO_Port, MOTOR_Y_PUL_Pin)
#define MOTOR_Y_DIR(x)        x ? LL_GPIO_SetOutputPin(MOTOR_Y_DIR_GPIO_Port, MOTOR_Y_DIR_Pin) : LL_GPIO_ResetOutputPin(MOTOR_Y_DIR_GPIO_Port, MOTOR_Y_DIR_Pin)


void turn_coordinate(float x, float y);
void drawline(float X0, float Y0, float Xe, float Ye);
void drawcircle(float x0, float y0, float R, uint8_t SorN);
void calibration(float xstep, float ystep);
void stepper_x_run(int tim,float step,float subdivide,uint8_t dir);
void stepper_y_run(int tim, float step, float subdivide, uint8_t dir);

#endif /* INC_STEPPER_CTRL_H_ */
