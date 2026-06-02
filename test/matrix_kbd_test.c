/**
 * \file      matrix_kbd_test.c
 * \brief     Unit tests for matrix_kbd driver (fully isolated)
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \version   0.2
 * \date      2024-01-14
 * \copyright (c) 2023 PassatInnovations LTD. All rights reserved.
 */

/*=============================[ INCLUDE ]================================*/

#include "matrix_kbd.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "unity.h"
#include "unity_fixture.h"

#include "matrix_kbd_osal_freertos.h"

#ifdef TEST_ON_HOST
    #include "matrix_kbd_hal_cmock.h"
    #include "mocks.h"
    #include <unistd.h>
#else
    #include "matrix_kbd_hal_asf.h"
#endif

/*=============================[ MACROS ]==================================*/

#define TEST_ROWS    4U
#define TEST_COLUMNS 4U

/*=============================[ DATA ]=====================================*/

static const uint8_t s_columnsPin [TEST_COLUMNS] = {1, 2, 3, 4};
static const uint8_t s_rowsPin [TEST_ROWS] = {5, 6, 7, 8};

static MatrixKbd_s s_kbd;
static MatrixKbdOsalFreertos_s s_osal;
static MatrixKbdHalPort_s s_hal;
static uint8_t s_parent;
static const char* s_name = "TEST_KBD";

/*=============================[ CALLBACKS ]================================*/

#ifdef TEST_ON_HOST
static MatrixKbdHalErr_e halPortRowStateGetCb(const void* hal, const uint8_t number, bool* data, int cmock_num_calls)
{
    (void) hal;
    (void) number;
    (void) cmock_num_calls;
    *data = true;
    return MATRIX_KBD_HAL_NO_ERR;
}
#endif

/*=============================[ HELPERS ]==================================*/

static void fullInit(void)
{
#ifdef TEST_ON_HOST
    MatrixKbdHalPortColumnDeselect_IgnoreAndReturn(MATRIX_KBD_HAL_NO_ERR);
    MatrixKbdHalPortColumnSelect_IgnoreAndReturn(MATRIX_KBD_HAL_NO_ERR);
    MatrixKbdHalPortRowStateGet_IgnoreAndReturn(MATRIX_KBD_HAL_NO_ERR);
#endif

    MatrixKbdOsalFreertosErr_e osalStatus = MatrixKbdOsalFreertosInit(&s_osal, NULL, NULL, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_FREERTOS_NO_ERR, osalStatus);

    MatrixKbdHalPortErr_e halStatus = MatrixKbdHalPortInit(&s_hal, TEST_COLUMNS, TEST_ROWS, s_columnsPin, s_rowsPin, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, halStatus);

    MatrixKbdErr_e kbdStatus = MatrixKbdInit(&s_kbd, (MatrixKbdOsal_s*) &s_osal, (MatrixKbdHal_s*) &s_hal, TEST_ROWS, TEST_COLUMNS, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, kbdStatus);
}

static void fullDeinit(void)
{
    MatrixKbdErr_e kbdStatus = MatrixKbdDeinit(&s_kbd);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, kbdStatus);

    MatrixKbdHalPortErr_e halStatus = MatrixKbdHalPortDeinit(&s_hal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, halStatus);

    MatrixKbdOsalFreertosErr_e osalStatus = MatrixKbdOsalFreertosDeinit(&s_osal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_OSAL_FREERTOS_NO_ERR, osalStatus);
}

/*=============================[ TEST GROUP ]===============================*/

TEST_GROUP(MATRIX_KBD);

TEST_SETUP(MATRIX_KBD)
{
    memset(&s_kbd, 0, sizeof(s_kbd));
    memset(&s_osal, 0, sizeof(s_osal));
    memset(&s_hal, 0, sizeof(s_hal));
    s_parent = 0;
    fullInit();
}

TEST_TEAR_DOWN(MATRIX_KBD)
{
    fullDeinit();
}

/*=============================[ TESTS ]====================================*/

TEST(MATRIX_KBD, Init_NullArgs)
{
    MatrixKbdErr_e status;

    status = MatrixKbdInit(NULL, (MatrixKbdOsal_s*) &s_osal, (MatrixKbdHal_s*) &s_hal, TEST_ROWS, TEST_COLUMNS, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdInit(&s_kbd, NULL, (MatrixKbdHal_s*) &s_hal, TEST_ROWS, TEST_COLUMNS, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdInit(&s_kbd, (MatrixKbdOsal_s*) &s_osal, NULL, TEST_ROWS, TEST_COLUMNS, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdInit(&s_kbd, (MatrixKbdOsal_s*) &s_osal, (MatrixKbdHal_s*) &s_hal, MATRIX_KBD_ROWS_COUNT_MAX + 1, TEST_COLUMNS, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);
}

TEST(MATRIX_KBD, ParentGet)
{
    void* parentToGet = NULL;

    MatrixKbdErr_e status = MatrixKbdParentGet(NULL, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdParentGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdParentGet(&s_kbd, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&s_parent, parentToGet);
}

TEST(MATRIX_KBD, ParentSet)
{
    uint8_t newParent = 42;

    MatrixKbdErr_e status = MatrixKbdParentSet(NULL, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdParentSet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdParentSet(&s_kbd, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);

    void* parentToGet = NULL;
    status = MatrixKbdParentGet(&s_kbd, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&newParent, parentToGet);
}

TEST(MATRIX_KBD, NameGet)
{
    char* nameToGet = NULL;

    MatrixKbdErr_e status = MatrixKbdNameGet(NULL, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdNameGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdNameGet(&s_kbd, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(s_name, nameToGet);
}

TEST(MATRIX_KBD, NameSet)
{
    const char* newName = "NEW_KBD";

    MatrixKbdErr_e status = MatrixKbdNameSet(NULL, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdNameSet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdNameSet(&s_kbd, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);

    char* nameToGet = NULL;
    status = MatrixKbdNameGet(&s_kbd, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(newName, nameToGet);
}

TEST(MATRIX_KBD, ColumnsCountGet)
{
    MatrixKbdColumn_t columns = 0;

    MatrixKbdErr_e status = MatrixKbdColumnsCountGet(NULL, &columns);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdColumnsCountGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdColumnsCountGet(&s_kbd, &columns);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_UINT8(TEST_COLUMNS, columns);
}

TEST(MATRIX_KBD, RowsCountGet)
{
    MatrixKbdRow_t rows = 0;

    MatrixKbdErr_e status = MatrixKbdRowsCountGet(NULL, &rows);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdRowsCountGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdRowsCountGet(&s_kbd, &rows);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_UINT8(TEST_ROWS, rows);
}

TEST(MATRIX_KBD, KeysCountGet)
{
    uint32_t keysCount = 0;

    MatrixKbdErr_e status = MatrixKbdKeysCountGet(NULL, &keysCount);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeysCountGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeysCountGet(&s_kbd, &keysCount);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_UINT32(TEST_ROWS * TEST_COLUMNS, keysCount);
}

TEST(MATRIX_KBD, KeyGet)
{
    MatrixKbdKey_s* key = NULL;

    MatrixKbdErr_e status = MatrixKbdKeyGet(NULL, 0, 0, &key);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyGet(&s_kbd, 0, 0, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyGet(&s_kbd, 0, 0, &key);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&s_kbd.keys [0][0], key);
}

TEST(MATRIX_KBD, KeyGetById)
{
    MatrixKbdKey_s* key = NULL;

    MatrixKbdErr_e status = MatrixKbdKeyGetById(NULL, 0, &key);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyGetById(&s_kbd, 0, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyGetById(&s_kbd, 0, &key);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&s_kbd.keys [0][0], key);
}

TEST(MATRIX_KBD, KeyIdGet)
{
    MatrixKbdKey_s* key = &s_kbd.keys [1][2];
    MatrixKbdKeyId_t id = 0;

    MatrixKbdErr_e status = MatrixKbdKeyIdGet(NULL, key, &id);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyIdGet(&s_kbd, NULL, &id);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyIdGet(&s_kbd, key, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyIdGet(&s_kbd, key, &id);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
}

TEST(MATRIX_KBD, KeyIdSet)
{
    MatrixKbdKey_s* key = &s_kbd.keys [0][0];
    MatrixKbdKeyId_t newId = 99;

    MatrixKbdErr_e status = MatrixKbdKeyIdSet(NULL, key, newId);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyIdSet(&s_kbd, NULL, newId);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyIdSet(&s_kbd, key, newId);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);

    MatrixKbdKeyId_t id = 0;
    status = MatrixKbdKeyIdGet(&s_kbd, key, &id);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_UINT8(newId, id);
}

TEST(MATRIX_KBD, KeyCbAttach)
{
    MatrixKbdKey_s* key = &s_kbd.keys [0][0];
    MatrixKbdKeyCb_f cb = (MatrixKbdKeyCb_f) 0x12345678;

    MatrixKbdErr_e status = MatrixKbdKeyCbAttach(NULL, key, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE, cb);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyCbAttach(&s_kbd, NULL, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE, cb);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyCbAttach(&s_kbd, key, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE, cb);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(cb, key->matrixKbdKeyUnpressedCb);
    TEST_ASSERT_NULL(key->matrixKbdKeyPressedCb);
    TEST_ASSERT_NULL(key->matrixKbdKeyPressingCb);
    TEST_ASSERT_NULL(key->matrixKbdKeyReleasedCb);
}

TEST(MATRIX_KBD, KeyCbDetach)
{
    MatrixKbdKey_s* key = &s_kbd.keys [0][0];
    MatrixKbdKeyCb_f cb = (MatrixKbdKeyCb_f) 0x12345678;

    key->matrixKbdKeyUnpressedCb = cb;

    MatrixKbdErr_e status = MatrixKbdKeyCbDetach(NULL, key, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyCbDetach(&s_kbd, NULL, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyCbDetach(&s_kbd, key, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_NULL(key->matrixKbdKeyUnpressedCb);
}

TEST(MATRIX_KBD, AllKeyCbAttach)
{
    MatrixKbdKeyCb_f cb = (MatrixKbdKeyCb_f) 0x12345678;

    MatrixKbdErr_e status = MatrixKbdAllKeyCbAttach(NULL, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE, cb);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdAllKeyCbAttach(&s_kbd, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE, cb);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(cb, s_kbd.matrixKbdAllKeyUnpressedCb);
}

TEST(MATRIX_KBD, AllKeyCbDetach)
{
    s_kbd.matrixKbdAllKeyUnpressedCb = (MatrixKbdKeyCb_f) 0x12345678;

    MatrixKbdErr_e status = MatrixKbdAllKeyCbDetach(NULL, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdAllKeyCbDetach(&s_kbd, MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
    TEST_ASSERT_NULL(s_kbd.matrixKbdAllKeyUnpressedCb);
}

TEST(MATRIX_KBD, KeyStateGet)
{
    MatrixKbdKey_s* key = &s_kbd.keys [0][0];
    MatrixKbdKeyState_t state = false;

    MatrixKbdErr_e status = MatrixKbdKeyStateGet(NULL, key, &state);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyStateGet(&s_kbd, NULL, &state);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyStateGet(&s_kbd, key, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyStateGet(&s_kbd, key, &state);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
}

TEST(MATRIX_KBD, KeysStateGet)
{
    MatrixKbdState_s state;

    MatrixKbdErr_e status = MatrixKbdKeysStateGet(NULL, &state);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeysStateGet(&s_kbd, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeysStateGet(&s_kbd, &state);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);
}

TEST(MATRIX_KBD, ScanStart)
{
    MatrixKbdErr_e status = MatrixKbdScanStart(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    const MatrixKbdOsal_s* savedOsal = s_kbd.osal;
    s_kbd.osal = NULL;
    status = MatrixKbdScanStart(&s_kbd);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NOT_INIT_ERR, status);
    s_kbd.osal = savedOsal;

#ifdef TEST_ON_HOST
    MatrixKbdHalPortRowStateGet_Stub(halPortRowStateGetCb);
#endif

    status = MatrixKbdScanStart(&s_kbd);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);

    MatrixKbdErr_e suspendStatus = MatrixKbdScanSuspend(&s_kbd);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, suspendStatus);
}

TEST(MATRIX_KBD, ScanSuspend)
{
    MatrixKbdErr_e status = MatrixKbdScanSuspend(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    const MatrixKbdOsal_s* savedOsal = s_kbd.osal;
    s_kbd.osal = NULL;
    status = MatrixKbdScanSuspend(&s_kbd);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NOT_INIT_ERR, status);
    s_kbd.osal = savedOsal;
}

TEST(MATRIX_KBD, KeyLastPressedGet)
{
    MatrixKbdKey_s* keys [MATRIX_KBD_KEY_LOG_SIZE] = {0};

    MatrixKbdErr_e status = MatrixKbdKeyLastPressedGet(NULL, keys, MATRIX_KBD_KEY_LOG_SIZE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyLastPressedGet(&s_kbd, NULL, MATRIX_KBD_KEY_LOG_SIZE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_INVALID_ARGS_ERR, status);

    status = MatrixKbdKeyLastPressedGet(&s_kbd, keys, MATRIX_KBD_KEY_LOG_SIZE);
    TEST_ASSERT_EQUAL(MATRIX_KBD_NO_ERR, status);

    for (uint8_t i = 0; i < MATRIX_KBD_KEY_LOG_SIZE; i++)
    {
        TEST_ASSERT_NULL(keys [i]);
    }
}

/*=============================[ TEST GROUP RUNNER ]========================*/

TEST_GROUP_RUNNER(MATRIX_KBD)
{
    RUN_TEST_CASE(MATRIX_KBD, Init_NullArgs);
    RUN_TEST_CASE(MATRIX_KBD, ParentGet);
    RUN_TEST_CASE(MATRIX_KBD, ParentSet);
    RUN_TEST_CASE(MATRIX_KBD, NameGet);
    RUN_TEST_CASE(MATRIX_KBD, NameSet);
    RUN_TEST_CASE(MATRIX_KBD, ColumnsCountGet);
    RUN_TEST_CASE(MATRIX_KBD, RowsCountGet);
    RUN_TEST_CASE(MATRIX_KBD, KeysCountGet);
    RUN_TEST_CASE(MATRIX_KBD, KeyGet);
    RUN_TEST_CASE(MATRIX_KBD, KeyGetById);
    RUN_TEST_CASE(MATRIX_KBD, KeyIdGet);
    RUN_TEST_CASE(MATRIX_KBD, KeyIdSet);
    RUN_TEST_CASE(MATRIX_KBD, KeyCbAttach);
    RUN_TEST_CASE(MATRIX_KBD, KeyCbDetach);
    RUN_TEST_CASE(MATRIX_KBD, AllKeyCbAttach);
    RUN_TEST_CASE(MATRIX_KBD, AllKeyCbDetach);
    RUN_TEST_CASE(MATRIX_KBD, KeyStateGet);
    RUN_TEST_CASE(MATRIX_KBD, KeysStateGet);
    RUN_TEST_CASE(MATRIX_KBD, ScanStart);
    RUN_TEST_CASE(MATRIX_KBD, ScanSuspend);
    RUN_TEST_CASE(MATRIX_KBD, KeyLastPressedGet);
}
