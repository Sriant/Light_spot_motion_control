/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t arv;
  float x1;
  float y1;
  float x2;
  float y2;
  float r;
}cmdinfopack;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId cmdtaskHandle;
osThreadId ctrltaskHandle;
osMessageQId cmd_QueueHandle;
osSemaphoreId cmd_BinarySemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void cmd_task(void const * argument);
void ctrl_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of cmd_BinarySem */
  osSemaphoreDef(cmd_BinarySem);
  cmd_BinarySemHandle = osSemaphoreCreate(osSemaphore(cmd_BinarySem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of cmd_Queue */
  osMessageQDef(cmd_Queue, 2, cmdinfopack);
  cmd_QueueHandle = osMessageCreate(osMessageQ(cmd_Queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of cmdtask */
  osThreadDef(cmdtask, cmd_task, osPriorityNormal, 0, 512);
  cmdtaskHandle = osThreadCreate(osThread(cmdtask), NULL);

  /* definition and creation of ctrltask */
  osThreadDef(ctrltask, ctrl_task, osPriorityIdle, 0, 512);
  ctrltaskHandle = osThreadCreate(osThread(ctrltask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_cmd_task */
/**
  * @brief  Function implementing the cmdtask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_cmd_task */
void cmd_task(void const * argument)
{
  /* USER CODE BEGIN cmd_task */
  BaseType_t xReturn = osErrorOS;
  char *token = NULL;
  char *arvg[5];
  const char *s = " ";                                 //命令参数分隔
  uint8_t cnt = 0;
  cmdinfopack pointinfo;

  osSemaphoreWait(cmd_BinarySemHandle, osWaitForever); //初始化后释放
  /* Infinite loop */
  for(;;)
  {
    xReturn = osSemaphoreWait(cmd_BinarySemHandle, osWaitForever);
    if(xReturn == osOK)
    {
      token = strtok(Usart1_Rx_Buf, s);
      while(token != NULL)
      {
        arvg[cnt] = token;
        token = strtok(NULL, s);
        cnt++;
      }

      if(strcmp(arvg[0], "setpoint") == 0)
      {
        pointinfo.arv = 1;
        sscanf(arvg[1], "%f,%f", &pointinfo.x1, &pointinfo.y1);
      }
      else if(strcmp(arvg[0], "drawline") == 0)
      {
        pointinfo.arv = 2;
        sscanf(arvg[1], "%f,%f", &pointinfo.x1, &pointinfo.y1);
        sscanf(arvg[2], "%f,%f", &pointinfo.x2, &pointinfo.y2);
      }
      else if(strcmp(arvg[0], "drawcircle") == 0)
      {
        pointinfo.arv = 3;
        sscanf(arvg[1], "%f,%f", &pointinfo.x1, &pointinfo.y1);
        sscanf(arvg[2], "%f", &pointinfo.r);
      }
      else if(strcmp(arvg[0], "calibration") == 0)
      {
        pointinfo.arv = 4;
        sscanf(arvg[1], "%f,%f", &pointinfo.x1, &pointinfo.y1);
      }
      else if(strcmp(arvg[0], "on") == 0)
      {
        pointinfo.arv = 5;
        MOTOR_X_EN(ON);
        MOTOR_Y_EN(ON);
      }
      else if(strcmp(arvg[0], "off") == 0)
      {
        pointinfo.arv = 6;
        MOTOR_X_EN(OFF);
        MOTOR_Y_EN(OFF);
      }

      osMessagePut(cmd_QueueHandle, (uint32_t)&pointinfo, 0);      //发指令消息给ctrl_task执行

      memset(Usart1_Rx_Buf, 0, USART1_RBUFF_SIZE);
      memset(*arvg, 0, 5);
      token = NULL;
      cnt = 0;
    }
  }
  /* USER CODE END cmd_task */
}

/* USER CODE BEGIN Header_ctrl_task */
/**
* @brief Function implementing the ctrltask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ctrl_task */
void ctrl_task(void const * argument)
{
  /* USER CODE BEGIN ctrl_task */
  osEvent cmdEvent;
  cmdinfopack *point;
  /* Infinite loop */
  for(;;)
  {
    cmdEvent = osMessageGet(cmd_QueueHandle, osWaitForever);    //等待指令消息
    if(cmdEvent.status == osEventMessage)
    {
      point = (cmdinfopack*)cmdEvent.value.p;
      switch(point->arv)
      {
      case 1:
        printf("show %d,%d\r\n", (int)point->x1, (int)point->y1);
        turn_coordinate(point->x1, point->y1);
        break;
      case 2:
        printf("show %d,%d\r\n", (int)point->x2, (int)point->y2);
        drawline(point->x1, point->y1, point->x2, point->y2);
        break;
      case 3:
        printf("show %d,%d\r\n", (int)point->x1, (int)(point->y1 + point->r));
        drawcircle(point->x1, point->y1, point->r, 1);
        break;
      case 4:
        calibration(point->x1, point->y1);
        break;
      default:break;
      }
      point->arv = 0;
    }
  }
  /* USER CODE END ctrl_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
