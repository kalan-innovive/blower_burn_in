//
// Created by Erik Klan on 1/31/24.
//
#ifdef __cplusplus
extern "C" {
#endif
#include "register.h"
#include "reg_types.h"
#include "dev_types.h"
#ifdef __cplusplus
}
#endif

#include "gtest/gtest.h"


TEST(Register, CreateObject) {
    int ret;
    device_valve_blower_t redev_t;
    ret = default_dev_handle(&redev_t);
    EXPECT_EQ(1, ret);
    EXPECT_EQ(UKNOWN_TYPE, redev_t.blower_type);
    EXPECT_EQ(DEF_DEV_HANDLE_CHIPID, redev_t.chip_id);
    EXPECT_EQ(DEF_DEV_CHIPID_VALID, redev_t.chip_id_valid);
    EXPECT_EQ(REG_CURR_PRESS, redev_t.cur_press.addr);
    EXPECT_EQ(REG_TARG_PRESS, redev_t.target_press.addr);
    EXPECT_EQ(REG_CALIBRATE, redev_t.cal.addr);
    EXPECT_EQ(REG_RAW_PRESS, redev_t.raw_press.addr);
    EXPECT_EQ(REG_RPM_A, redev_t.valve_position.addr);

}


TEST(Register, CreateObject_FAIL) {
    int ret;
    device_valve_blower_t *redev_t;
    redev_t = NULL;
    ret = default_dev_handle(redev_t);
    EXPECT_EQ(-1, ret);
}


TEST(Register, CreateList_momory) {
    reg_list *reg_list;
    int len = 5;
    reg_list = create_reg_list(len);
    EXPECT_TRUE(NULL != reg_list);
    EXPECT_TRUE(NULL != reg_list->reg_list);
    free(reg_list->reg_list);
    free(reg_list);
}


TEST(DestructRegListTest, FreesMemoryProperly) {
    // Test that the function frees memory as expected
    reg_list *test_list = (reg_list *) malloc(sizeof(reg_list));
    ASSERT_NE(test_list, nullptr); // Ensure memory allocation was successful

    // Optionally initialize test_list or its members
    test_list->reg_list = (register_value_t *) malloc(sizeof(register_value_t) * 10); // Example allocation
    ASSERT_NE(test_list->reg_list, nullptr); // Ensure memory allocation was successful

    destruct_reg_list(test_list);

}

TEST(DefaultRegisterTest, SetsDefaultValuesCorrectly) {
    register_value_t reg;
    int testId = 1; // Example ID to test

    // Initialize reg with some non-default values
    reg.addr = REG_NV_STORE_LOW;
    reg.val = 123;
    reg.write_val = 456;
    reg.curr_value_status = REG_VALID;
    reg.update_status = REG_VALID;

    // Call the function under test
    default_register(&reg, testId);

    // Assert that all fields are set to their expected default values
    EXPECT_EQ(reg.addr, (blower_reg_addr) testId);
    EXPECT_EQ(reg.val, DEFAULT_REG_VAL);
    EXPECT_EQ(reg.write_val, DEFAULT_REG_VAL);
    EXPECT_EQ(reg.curr_value_status, REG_NOT_UPDATED);
    EXPECT_EQ(reg.update_status, UKNOWN_REG_STATUS);
}

