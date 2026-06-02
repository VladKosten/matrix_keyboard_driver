/**
 * \file      matrix_kbd_osal_test.c
 * \brief     Unit tests for OSAL module (fully isolated)
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \version   0.2
 * \date      2024-01-14
 * \copyright (c) 2023 PassatInnovations LTD. All rights reserved.
 */

/*=============================[ INCLUDE ]================================*/

#include "matrix_kbd_osal.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "unity.h"
#include "unity_fixture.h"

/*=============================[ DATA ]=====================================*/

static MatrixKbdOsal_s s_osal;
static uint8_t s_parent;
static const char* s_name = "TEST_OSAL";

/*=============================[ DUMMY FUNCTIONS ]==========================*/

static void threadWorkerDummy(const void* kbd)
{
    (void) kbd;
}

static MatrixKbdOsalErr_e threadResumeDummy(const void* osal)
{
    TEST_ASSERT_NOT_NULL(osal);
    return MATRIX_KBD_OSAL_NO_ERR;
}

static MatrixKbdOsalErr_e threadSuspendDummy(const void* osal)
{
    TEST_ASSERT_NOT_NULL(osal);
    return MATRIX_KBD_OSAL_NO_ERR;
}

static MatrixKbdOsalErr_e threadDelayDummy(const void* osal, const uint32_t msDelay)
{
    TEST_ASSERT_NOT_NULL(osal);
    (void) msDelay;
    return MATRIX_KBD_OSAL_NO_ERR;
}

static MatrixKbdOsalErr_e lockDummy(const void* osal)
{
    TEST_ASSERT_NOT_NULL(osal);
    return MATRIX_KBD_OSAL_NO_ERR;
}

static MatrixKbdOsalErr_e unlockDummy(const void* osal)
{
    TEST_ASSERT_NOT_NULL(osal);
    return MATRIX_KBD_OSAL_NO_ERR;
}

static const MatrixKbdOsalPortable_s s_portableDummy =
    {
        .threadResume = threadResumeDummy,
        .threadSuspend = threadSuspendDummy,
        .threadDelay = threadDelayDummy,
        .lock = lockDummy,
        .unlock = unlockDummy,
};

/*=============================[ TEST GROUP ]===============================*/

TEST_GROUP(MATRIX_KBD_OSAL);

TEST_SETUP(MATRIX_KBD_OSAL)
{
    memset(&s_osal, 0, sizeof(s_osal));
    s_parent = 0;
    MatrixKbdOsalErr_e status = MatrixKbdOsalInit(&s_osal, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    s_osal.portable = &s_portableDummy;
    s_osal.threadHandle = (void*) 0x1;
    s_osal.mutexHandle = (void*) 0x1;
}

TEST_TEAR_DOWN(MATRIX_KBD_OSAL)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalDeinit(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

/*=============================[ TESTS ]====================================*/

TEST(MATRIX_KBD_OSAL, Init_NullArgs)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalInit(NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);
}

TEST(MATRIX_KBD_OSAL, Deinit_NullArgs)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalDeinit(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);
}

TEST(MATRIX_KBD_OSAL, ParentGet)
{
    void* parentToGet = NULL;

    MatrixKbdOsalErr_e status = MatrixKbdOsalParentGet(NULL, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalParentGet(&s_osal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalParentGet(&s_osal, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&s_parent, parentToGet);
}

TEST(MATRIX_KBD_OSAL, ParentSet)
{
    uint8_t newParent = 42;

    MatrixKbdOsalErr_e status = MatrixKbdOsalParentSet(NULL, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalParentSet(&s_osal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalParentSet(&s_osal, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);

    void* parentToGet = NULL;
    status = MatrixKbdOsalParentGet(&s_osal, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&newParent, parentToGet);
}

TEST(MATRIX_KBD_OSAL, NameGet)
{
    char* nameToGet = NULL;

    MatrixKbdOsalErr_e status = MatrixKbdOsalNameGet(NULL, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalNameGet(&s_osal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalNameGet(&s_osal, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(s_name, nameToGet);
}

TEST(MATRIX_KBD_OSAL, NameSet)
{
    const char* newName = "NEW_OSAL";

    MatrixKbdOsalErr_e status = MatrixKbdOsalNameSet(NULL, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalNameSet(&s_osal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalNameSet(&s_osal, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);

    char* nameToGet = NULL;
    status = MatrixKbdOsalNameGet(&s_osal, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(newName, nameToGet);
}

TEST(MATRIX_KBD_OSAL, ThreadResume)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalThreadResume(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    const MatrixKbdOsalPortable_s* savedPortable = s_osal.portable;
    s_osal.portable = NULL;
    status = MatrixKbdOsalThreadResume(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NOT_INIT_ERR, status);
    s_osal.portable = savedPortable;

    status = MatrixKbdOsalThreadResume(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

TEST(MATRIX_KBD_OSAL, ThreadSuspend)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalThreadSuspend(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    const MatrixKbdOsalPortable_s* savedPortable = s_osal.portable;
    s_osal.portable = NULL;
    status = MatrixKbdOsalThreadSuspend(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NOT_INIT_ERR, status);
    s_osal.portable = savedPortable;

    status = MatrixKbdOsalThreadSuspend(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

TEST(MATRIX_KBD_OSAL, ThreadWorkAttach)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalThreadWorkerAttach(NULL, threadWorkerDummy);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalThreadWorkerAttach(&s_osal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalThreadWorkerAttach(&s_osal, threadWorkerDummy);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(threadWorkerDummy, s_osal.threadWorker);
}

TEST(MATRIX_KBD_OSAL, ThreadWorkerDetach)
{
    s_osal.threadWorker = threadWorkerDummy;

    MatrixKbdOsalErr_e status = MatrixKbdOsalThreadWorkerDetach(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdOsalThreadWorkerDetach(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
    TEST_ASSERT_NULL(s_osal.threadWorker);
}

TEST(MATRIX_KBD_OSAL, ThreadDelay)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalThreadDelay(NULL, 100);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    const MatrixKbdOsalPortable_s* savedPortable = s_osal.portable;
    s_osal.portable = NULL;
    status = MatrixKbdOsalThreadDelay(&s_osal, 100);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NOT_INIT_ERR, status);
    s_osal.portable = savedPortable;

    status = MatrixKbdOsalThreadDelay(&s_osal, 100);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

TEST(MATRIX_KBD_OSAL, Lock)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalLock(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    const MatrixKbdOsalPortable_s* savedPortable = s_osal.portable;
    s_osal.portable = NULL;
    status = MatrixKbdOsalLock(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NOT_INIT_ERR, status);
    s_osal.portable = savedPortable;

    status = MatrixKbdOsalLock(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

TEST(MATRIX_KBD_OSAL, Unlock)
{
    MatrixKbdOsalErr_e status = MatrixKbdOsalUnlock(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_INVALID_ARGS_ERR, status);

    const MatrixKbdOsalPortable_s* savedPortable = s_osal.portable;
    s_osal.portable = NULL;
    status = MatrixKbdOsalUnlock(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NOT_INIT_ERR, status);
    s_osal.portable = savedPortable;

    status = MatrixKbdOsalUnlock(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_NO_ERR, status);
}

/*=============================[ TEST GROUP RUNNER ]========================*/

TEST_GROUP_RUNNER(MATRIX_KBD_OSAL)
{
    RUN_TEST_CASE(MATRIX_KBD_OSAL, Init_NullArgs);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, Deinit_NullArgs);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ParentGet);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ParentSet);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, NameGet);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, NameSet);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ThreadResume);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ThreadSuspend);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ThreadWorkAttach);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ThreadWorkerDetach);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, ThreadDelay);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, Lock);
    RUN_TEST_CASE(MATRIX_KBD_OSAL, Unlock);
}
