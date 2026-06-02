/**
 * \file      matrix_kbd_osal_freertos_test.c
 * \brief     Unit test for osal freertos module of matrix kbd
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */

//===============================================================================[ INCLUDE ]========================================================================================

#include "matrix_kbd_osal_freertos.h"

#include <stdint.h>
#include <stddef.h>

#include "unity.h"
#include "unity_fixture.h"

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

static MatrixKbdOsalFreertos_s osalFreeRtosDummy = {0};

//===============================================================[ TEST GROUPS ]=====================================================================

TEST_GROUP(MATRIX_KBD_OSAL_FREERTOS);

//===============================================================[ SETUP / TEARDOWN ]=====================================================================

TEST_SETUP(MATRIX_KBD_OSAL_FREERTOS)
{
}

TEST_TEAR_DOWN(MATRIX_KBD_OSAL_FREERTOS)
{
}

//===============================================================[ TESTS ]=====================================================================

TEST(MATRIX_KBD_OSAL_FREERTOS, Init)
{
    TEST_MESSAGE("MatrixKbdOsalFreertos init invalid arg: ...");
    MatrixKbdOsalFreertosErr_e osalFreeRtosStatus = MatrixKbdOsalFreertosInit(NULL, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT32(MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR, osalFreeRtosStatus);

    TEST_MESSAGE("MatrixKbdOsalFreertos init valid arg: ...");
    MatrixKbdOsalFreertos_s* osalFreeRtos = &osalFreeRtosDummy;
    osalFreeRtosStatus = MatrixKbdOsalFreertosInit(osalFreeRtos, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT32(MATRIX_KBD_OSAL_FREERTOS_NO_ERR, osalFreeRtosStatus);
    TEST_ASSERT_NULL(osalFreeRtos->base.threadWorker);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.mutexHandle);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.threadHandle);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->lock);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadDelay);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadResume);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadSuspend);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->unlock);
}

TEST(MATRIX_KBD_OSAL_FREERTOS, Deinit)
{
    MatrixKbdOsalFreertos_s* osalFreeRtos = &osalFreeRtosDummy;

    TEST_MESSAGE("MatrixKbdOsalFreertos deinit invalid arg: ...");
    MatrixKbdOsalFreertosErr_e osalFreeRtosStatus = MatrixKbdOsalFreertosDeinit(NULL);
    TEST_ASSERT_EQUAL_UINT32(MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR, osalFreeRtosStatus);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.mutexHandle);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.threadHandle);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->lock);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadDelay);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadResume);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->threadSuspend);
    TEST_ASSERT_NOT_NULL(osalFreeRtos->base.portable->unlock);

    TEST_MESSAGE("MatrixKbdOsalFreertos deinit valid arg: ...");
    osalFreeRtosStatus = MatrixKbdOsalFreertosDeinit(osalFreeRtos);
    TEST_ASSERT_EQUAL_UINT32(MATRIX_KBD_OSAL_FREERTOS_NO_ERR, osalFreeRtosStatus);
    TEST_ASSERT_NULL(osalFreeRtos->base.portable);
    TEST_ASSERT_NULL(osalFreeRtos->base.parent);
    TEST_ASSERT_NULL(osalFreeRtos->base.mutexHandle);
    TEST_ASSERT_NULL(osalFreeRtos->base.threadHandle);
    TEST_ASSERT_NULL(osalFreeRtos->base.threadWorker);
}

//===============================================================[ TEST GROUP RUNNER ]=====================================================================

TEST_GROUP_RUNNER(MATRIX_KBD_OSAL_FREERTOS)
{
    RUN_TEST_CASE(MATRIX_KBD_OSAL_FREERTOS, Init);
    RUN_TEST_CASE(MATRIX_KBD_OSAL_FREERTOS, Deinit);
}