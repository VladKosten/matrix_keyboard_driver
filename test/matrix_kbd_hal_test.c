/**
 * \file      matrix_kbd_hal_test.c
 * \brief     Unit tests for HAL module (fully isolated)
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \version   0.2
 * \date      2024-01-14
 * \copyright (c) 2023 PassatInnovations LTD. All rights reserved.
 */

/*=============================[ INCLUDE ]================================*/

#include "matrix_kbd_hal.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "unity.h"
#include "unity_fixture.h"

/*=============================[ DATA ]=====================================*/

static MatrixKbdHal_s s_hal;
static uint8_t s_parent;
static const char* s_name = "TEST_HAL";

/*=============================[ DUMMY FUNCTIONS ]==========================*/

static MatrixKbdHalErr_e columnSelectDummy(const void* hal, const uint8_t number)
{
    TEST_ASSERT_NOT_NULL(hal);
    (void) number;
    return MATRIX_KBD_HAL_NO_ERR;
}

static MatrixKbdHalErr_e columnDeselectDummy(const void* hal, const uint8_t number)
{
    TEST_ASSERT_NOT_NULL(hal);
    (void) number;
    return MATRIX_KBD_HAL_NO_ERR;
}

static MatrixKbdHalErr_e rowStateGetDummy(const void* hal, const uint8_t number, bool* data)
{
    TEST_ASSERT_NOT_NULL(hal);
    TEST_ASSERT_NOT_NULL(data);
    (void) number;
    *data = true;
    return MATRIX_KBD_HAL_NO_ERR;
}

static MatrixKbdHalErr_e columnSelectError(const void* hal, const uint8_t number)
{
    (void) hal;
    (void) number;
    return MATRIX_KBD_HAL_PORT_LAYER_ERR;
}

static MatrixKbdHalErr_e columnDeselectError(const void* hal, const uint8_t number)
{
    (void) hal;
    (void) number;
    return MATRIX_KBD_HAL_PORT_LAYER_ERR;
}

static MatrixKbdHalErr_e rowStateGetError(const void* hal, const uint8_t number, bool* data)
{
    (void) hal;
    (void) number;
    (void) data;
    return MATRIX_KBD_HAL_PORT_LAYER_ERR;
}

static const MatrixKbdHalPortable_s s_portableDummy =
    {
        .columnSelect = columnSelectDummy,
        .columnDeselect = columnDeselectDummy,
        .rowStateGet = rowStateGetDummy,
};

/*=============================[ TEST GROUP ]===============================*/

TEST_GROUP(MATRIX_KBD_HAL);

TEST_SETUP(MATRIX_KBD_HAL)
{
    memset(&s_hal, 0, sizeof(s_hal));
    s_parent = 0;
    MatrixKbdHalErr_e status = MatrixKbdHalInit(&s_hal, &s_parent, s_name);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    s_hal.portable = &s_portableDummy;
}

TEST_TEAR_DOWN(MATRIX_KBD_HAL)
{
    MatrixKbdHalErr_e status = MatrixKbdHalDeinit(&s_hal);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
}

/*=============================[ TESTS ]====================================*/

TEST(MATRIX_KBD_HAL, Init_NullArgs)
{
    MatrixKbdHalErr_e status = MatrixKbdHalInit(NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);
}

TEST(MATRIX_KBD_HAL, Deinit_NullArgs)
{
    MatrixKbdHalErr_e status = MatrixKbdHalDeinit(NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);
}

TEST(MATRIX_KBD_HAL, ParentGet)
{
    void* parentToGet = NULL;

    MatrixKbdHalErr_e status = MatrixKbdHalParentGet(NULL, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalParentGet(&s_hal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalParentGet(&s_hal, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&s_parent, parentToGet);
}

TEST(MATRIX_KBD_HAL, ParentSet)
{
    uint8_t newParent = 42;

    MatrixKbdHalErr_e status = MatrixKbdHalParentSet(NULL, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalParentSet(&s_hal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalParentSet(&s_hal, &newParent);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);

    void* parentToGet = NULL;
    status = MatrixKbdHalParentGet(&s_hal, &parentToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_PTR(&newParent, parentToGet);
}

TEST(MATRIX_KBD_HAL, NameGet)
{
    char* nameToGet = NULL;

    MatrixKbdHalErr_e status = MatrixKbdHalNameGet(NULL, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalNameGet(&s_hal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalNameGet(&s_hal, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(s_name, nameToGet);
}

TEST(MATRIX_KBD_HAL, NameSet)
{
    const char* newName = "NEW_HAL";

    MatrixKbdHalErr_e status = MatrixKbdHalNameSet(NULL, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalNameSet(&s_hal, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalNameSet(&s_hal, newName);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);

    char* nameToGet = NULL;
    status = MatrixKbdHalNameGet(&s_hal, &nameToGet);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    TEST_ASSERT_EQUAL_STRING(newName, nameToGet);
}

TEST(MATRIX_KBD_HAL, ColumnSelect)
{
    s_hal.portable = &s_portableDummy;

    MatrixKbdHalErr_e status = MatrixKbdHalColumnSelect(NULL, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    const MatrixKbdHalPortable_s* savedPortable = s_hal.portable;
    s_hal.portable = NULL;
    status = MatrixKbdHalColumnSelect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
    s_hal.portable = savedPortable;

    const MatrixKbdHalPortable_s errorPortable = {
        .columnSelect = columnSelectError,
        .columnDeselect = columnDeselectDummy,
        .rowStateGet = rowStateGetDummy,
    };
    s_hal.portable = &errorPortable;
    status = MatrixKbdHalColumnSelect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_PORT_LAYER_ERR, status);
    s_hal.portable = savedPortable;

    status = MatrixKbdHalColumnSelect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
}

TEST(MATRIX_KBD_HAL, ColumnDeselect)
{
    s_hal.portable = &s_portableDummy;

    MatrixKbdHalErr_e status = MatrixKbdHalColumnDeselect(NULL, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    const MatrixKbdHalPortable_s* savedPortable = s_hal.portable;
    s_hal.portable = NULL;
    status = MatrixKbdHalColumnDeselect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
    s_hal.portable = savedPortable;

    const MatrixKbdHalPortable_s errorPortable = {
        .columnSelect = columnSelectDummy,
        .columnDeselect = columnDeselectError,
        .rowStateGet = rowStateGetDummy,
    };
    s_hal.portable = &errorPortable;
    status = MatrixKbdHalColumnDeselect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_PORT_LAYER_ERR, status);
    s_hal.portable = savedPortable;

    status = MatrixKbdHalColumnDeselect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
}

TEST(MATRIX_KBD_HAL, RowStateGet)
{
    bool data = false;
    s_hal.portable = &s_portableDummy;

    MatrixKbdHalErr_e status = MatrixKbdHalRowStateGet(NULL, 0, &data);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    status = MatrixKbdHalRowStateGet(&s_hal, 0, NULL);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_INVALID_ARGS_ERR, status);

    const MatrixKbdHalPortable_s* savedPortable = s_hal.portable;
    s_hal.portable = NULL;
    status = MatrixKbdHalRowStateGet(&s_hal, 0, &data);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
    s_hal.portable = savedPortable;

    const MatrixKbdHalPortable_s errorPortable = {
        .columnSelect = columnSelectDummy,
        .columnDeselect = columnDeselectDummy,
        .rowStateGet = rowStateGetError,
    };
    s_hal.portable = &errorPortable;
    status = MatrixKbdHalRowStateGet(&s_hal, 0, &data);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_PORT_LAYER_ERR, status);
    s_hal.portable = savedPortable;

    status = MatrixKbdHalRowStateGet(&s_hal, 0, &data);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NO_ERR, status);
    TEST_ASSERT_TRUE(data);
}

TEST(MATRIX_KBD_HAL, ColumnSelect_NullFunc)
{
    const MatrixKbdHalPortable_s partialPortable = {
        .columnSelect = NULL,
        .columnDeselect = columnDeselectDummy,
        .rowStateGet = rowStateGetDummy,
    };
    s_hal.portable = &partialPortable;
    MatrixKbdHalErr_e status = MatrixKbdHalColumnSelect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
}

TEST(MATRIX_KBD_HAL, ColumnDeselect_NullFunc)
{
    const MatrixKbdHalPortable_s partialPortable = {
        .columnSelect = columnSelectDummy,
        .columnDeselect = NULL,
        .rowStateGet = rowStateGetDummy,
    };
    s_hal.portable = &partialPortable;
    MatrixKbdHalErr_e status = MatrixKbdHalColumnDeselect(&s_hal, 0);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
}

TEST(MATRIX_KBD_HAL, RowStateGet_NullFunc)
{
    const MatrixKbdHalPortable_s partialPortable = {
        .columnSelect = columnSelectDummy,
        .columnDeselect = columnDeselectDummy,
        .rowStateGet = NULL,
    };
    s_hal.portable = &partialPortable;
    bool data = false;
    MatrixKbdHalErr_e status = MatrixKbdHalRowStateGet(&s_hal, 0, &data);
    TEST_ASSERT_EQUAL(MATRIX_KBD_HAL_NOT_INIT_ERR, status);
}

/*=============================[ TEST GROUP RUNNER ]========================*/

TEST_GROUP_RUNNER(MATRIX_KBD_HAL)
{
    RUN_TEST_CASE(MATRIX_KBD_HAL, Init_NullArgs);
    RUN_TEST_CASE(MATRIX_KBD_HAL, Deinit_NullArgs);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ParentGet);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ParentSet);
    RUN_TEST_CASE(MATRIX_KBD_HAL, NameGet);
    RUN_TEST_CASE(MATRIX_KBD_HAL, NameSet);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ColumnSelect);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ColumnDeselect);
    RUN_TEST_CASE(MATRIX_KBD_HAL, RowStateGet);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ColumnSelect_NullFunc);
    RUN_TEST_CASE(MATRIX_KBD_HAL, ColumnDeselect_NullFunc);
    RUN_TEST_CASE(MATRIX_KBD_HAL, RowStateGet_NullFunc);
}
