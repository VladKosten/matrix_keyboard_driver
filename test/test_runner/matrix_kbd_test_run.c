/**
 * \file      matrix_kbd_test_run.c
 * \brief     Unit test runner for matrix keyboard driver
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \version   0.2
 * \date      2024-01-14
 * \copyright (c) 2023 PassatInnovations LTD. All rights reserved.
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "unity.h"
#include "unity_fixture.h"

#include "FreeRTOS.h"
#include "task.h"

/*========================================================[ INTERNAL DATA TYPES ]===========================================*/

typedef struct
{
    int argc;
    const char** argv;
} TestRunnerArgs_t;

/*========================================================[ INTERNAL DATA ]=================================================*/

static TestRunnerArgs_t s_args;
static int s_exitCode = 0;

/*========================================================[ INTERNAL FUNCTIONS ]============================================*/

static void vApplicationIdleHook(void)
{
    usleep(15000);
}

static void vTestRunnerTask(void* pvParameters)
{
    TestRunnerArgs_t* args = (TestRunnerArgs_t*)pvParameters;

    s_exitCode = UnityMain(args->argc, args->argv, vTestRunnerRunAll);

    vTaskEndScheduler();
}

static void vTestRunnerRunAll(void)
{
    RUN_TEST_GROUP(MATRIX_KBD_OSAL);
    RUN_TEST_GROUP(MATRIX_KBD_OSAL_FREERTOS);
    RUN_TEST_GROUP(MATRIX_KBD_HAL);
    RUN_TEST_GROUP(MATRIX_KBD);
}

/*========================================================[ PUBLIC FUNCTIONS ]=============================================*/

int main(int argc, const char** argv)
{
    s_args.argc = argc;
    s_args.argv = argv;

    BaseType_t xResult = xTaskCreate(vTestRunnerTask,
                                     "TEST",
                                     configMINIMAL_STACK_SIZE,
                                     &s_args,
                                     configMAX_PRIORITIES - 2,
                                     NULL);
    configASSERT(xResult == pdPASS);

    vTaskStartScheduler();

    return s_exitCode;
}
