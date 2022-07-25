/*
 * stepper_ctrl.c
 *
 *  Created on: May 7, 2021
 *      Author: YAN
 */

#include "stepper_ctrl.h"
#include "cmsis_os.h"

#define Step_one 1 //单步步进值

static float now_x_step = 0, now_y_step = 0;

void turn_coordinate(float x, float y)
{
  float angle_x, angle_y;
  float step_x, step_y;
  float dx, dy;
  float sqx;

  arm_sqrt_f32(1050 * 1050 + x * x, &sqx);
  angle_x = atan(x / 1050) * 180 / PI;
  angle_y = atan(y / sqx) * 180 / PI;

  step_x = angle_x / 0.05625;//计算对应步数,与0相差
  step_y = angle_y / 0.05625;

  dx = step_x - now_x_step;
  dy = step_y - now_y_step;

  if(dx > 0)
    stepper_x_run(2, dx, 32, CCW);
  else if(dx < 0)
    stepper_x_run(2, -dx, 32, CW);

  if(dy > 0)
    stepper_y_run(2, dy, 32, CW);
  else if(dy < 0)
    stepper_y_run(2, -dy, 32, CCW);
}
/*
 * @brief：直线运动插补
 * @parameter：起点坐标（X0, Y0），终点坐标（Xe, Ye）
 * @return： 无
 * */
void drawline(float X0, float Y0, float Xe, float Ye)
{
  float NXY;              //总步数
  float Fm = 0;           //偏差
  float Xm = X0, Ym = Y0; //当前坐标
  uint8_t XOY;            //象限

  Xe = Xe - X0;
  Ye = Ye - Y0;
  NXY = (fabsf(Xe) + fabsf(Ye)) / Step_one;

  if(Xe > 0 && Ye >= 0) XOY = 1;
  else if(Xe <= 0 && Ye > 0) XOY = 2;
  else if(Xe < 0 && Ye <= 0) XOY = 3;
  else if(Xe >= 0 && Ye < 0) XOY = 4;

  while(NXY > 0)
  {
    switch (XOY)
    {
    case 1: (Fm >= 0) ? (Xm += Step_one) : (Ym += Step_one); break;
    case 2: (Fm <  0) ? (Xm -= Step_one) : (Ym += Step_one); break;
    case 3: (Fm >= 0) ? (Xm -= Step_one) : (Ym -= Step_one); break;
    case 4: (Fm <  0) ? (Xm += Step_one) : (Ym -= Step_one); break;
    default: break;
    }
    NXY -= 1;
    Fm = (Ym - Y0) * Xe - (Xm - X0) * Ye;
    turn_coordinate(Xm, Ym);
    osDelay(2);
  }
}
/*
 * @brief：圆弧运动插补
 * @parameter：圆心坐标（x0, y0），半径 R, 方向 SorN 1 顺时针 2 逆时针
 * @return： 无
 * */
void drawcircle(float x0, float y0, float R, uint8_t SorN)
{
  float X0, Y0, Xe, Ye;
  float step = 0;
  float Fm = 0;
  float Xm, Ym;
  uint8_t XOY;

  X0 = x0; Y0 = y0 + R;
  Xe = x0; Ye = y0 + R;
  Xm = X0; Ym = Y0;

  while (pow((Xm - Xe), 2) + pow((Ym - Ye), 2) > Step_one * Step_one / 2 || (step == 0))
  {
    if ((Xm - x0) > 0 && (Ym - y0) >= 0) XOY = 1;
    else if ((Xm - x0) <= 0 && (Ym - y0) > 0) XOY = 2;
    else if ((Xm - x0) < 0 && (Ym - y0) <= 0) XOY = 3;
    else if ((Xm - x0) >= 0 && (Ym - y0) < 0) XOY = 4;

    switch (XOY)
    {
    case 1:
      if(SorN == 1)
        (Fm >= 0) ? (Ym -= Step_one) : (Xm += Step_one);
      else
        (Fm <= 0) ? (Ym += Step_one) : (Xm -= Step_one);
      break;
    case 2:
      if(SorN == 1)
        (Fm >= 0) ? (Xm += Step_one) : (Ym += Step_one);
      else
        (Fm >  0) ? (Ym -= Step_one) : (Xm -= Step_one);
      break;
    case 3:
      if(SorN == 1)
        (Fm >= 0) ? (Ym += Step_one) : (Xm -= Step_one);
      else
        (Fm >  0) ? (Xm += Step_one) : (Ym -= Step_one);
      break;
    case 4:
      if(SorN == 1)
        (Fm >= 0) ? (Xm -= Step_one) : (Ym -= Step_one);
      else
        (Fm >  0) ? (Ym += Step_one) : (Xm += Step_one);
    default: break;
    }
    step = step + 1;
    Fm = pow((Xm - x0), 2) + pow((Ym - y0), 2) - pow(R, 2);
    turn_coordinate(Xm, Ym);
    osDelay(2);
  }
}
/**
 * 手动校准光点不记忆步数
 */
void calibration(float xstep, float ystep)
{
  if(xstep > 0)
    stepper_x_run(2, xstep, 32, CCW);
  else if(xstep < 0)
    stepper_x_run(2, -xstep, 32, CW);

  if(ystep > 0)
    stepper_y_run(2, ystep, 32, CW);
  else if(ystep < 0)
    stepper_y_run(2, -ystep, 32, CCW);

  now_x_step = 0, now_y_step = 0;
}
/*
 * x
 * CCW正方向
 */
void stepper_x_run(int tim,float step,float subdivide,uint8_t dir)
{
  int i;
  if(step < 0.5)
    return;
  if(dir == CW)
    MOTOR_X_DIR(CW);
  else if(dir == CCW)
    MOTOR_X_DIR(CCW);
  osDelay(2);
  for(i = 0; i < step; i++)
  {
    if(dir == CW)
      now_x_step--;
    else if(dir == CCW)
      now_x_step++;
    MOTOR_X_PUL(HIGH);
    osDelay(tim / 2);
    MOTOR_X_PUL(LOW);
    osDelay(tim / 2);
  }
}
/*
 * y
 * CW正方向
 */
void stepper_y_run(int tim, float step, float subdivide, uint8_t dir)
{
  int i;
  if(step < 0.5)
    return;
  if(dir == CW)
    MOTOR_Y_DIR(CW);
  else if(dir == CCW)
    MOTOR_Y_DIR(CCW);
  osDelay(2);
  for(i = 0; i < step; i++)
  {
    if(dir == CW)
      now_y_step++;
    else if(dir == CCW)
      now_y_step--;

    MOTOR_Y_PUL(HIGH);
    osDelay(tim / 2);
    MOTOR_Y_PUL(LOW);
    osDelay(tim / 2);
  }
}
