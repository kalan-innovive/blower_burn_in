//
// Created by Erik Klan on 3/17/24.
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

// Test fixture class
class DeviceValveBlowerTest : public ::testing::Test {
protected:
    device_valve_blower_t *b_handle;

    void SetUp() override {
        // Allocate memory for b_handle before each test
        b_handle = (device_valve_blower_t *) malloc(sizeof(device_valve_blower_t));
    }

    void TearDown() override {
        // Clean up after each test
        delete b_handle;
    }
};

// Test to check behavior with NULL pointer
TEST_F(DeviceValveBlowerTest, HandleNullPointer) {
    int result = default_dev_handle(nullptr); // Use nullptr instead of NULL in C++
    ASSERT_EQ(result, -1);
}

// Test to verify the correct initialization of device_valve_blower_t
TEST_F(DeviceValveBlowerTest, CorrectInitialization) {
    int result = default_dev_handle(b_handle);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(b_handle->blower_type, UKNOWN_TYPE);
    ASSERT_EQ(b_handle->chip_id, 0u);
    ASSERT_EQ(b_handle->chip_id_valid, 0u);
    ASSERT_EQ(b_handle->cur_press.addr, REG_CURR_PRESS);
    ASSERT_EQ(b_handle->target_press.addr, REG_TARG_PRESS);
    ASSERT_EQ(b_handle->cal.addr, REG_CALIBRATE);
    ASSERT_EQ(b_handle->raw_press.addr, REG_RAW_PRESS);
    ASSERT_EQ(b_handle->valve_position.addr, REG_RPM_A);
}

TEST_F(DeviceValveBlowerTest, CreateDeviceHandle) {
    // Call the function to test
    device_valve_blower_t *device_handle = create_device_handle();

    // Verify the function returns a non-NULL pointer
    ASSERT_NE(device_handle, nullptr);

    // verify the object has been correctly initialized at least the important fields
    // since these checks are similar to those in the CorrectInitialization test
    ASSERT_EQ(device_handle->blower_type, UKNOWN_TYPE);
    ASSERT_EQ(device_handle->chip_id, 0u);
    ASSERT_EQ(device_handle->chip_id_valid, 0u);

    // Free the allocated memory to avoid memory leak
    destruct_device_handle(&device_handle);
    ASSERT_EQ(device_handle, nullptr);
}

#include "gmock/gmock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgPointee;
using ::testing::DoAll;

// Mock class for transaction function
class MockTransact {
public:
    MOCK_METHOD(int, transact, (msg16_t * req, msg16_t * resp, int timeout));

    int moc_transaction_func(msg16_t *req, msg16_t *resp, int timeout) {
        // Implement the mock behavior here
        if (req->type != WRITE_REQ) {
            resp->dev_id = req->dev_id;
            resp->addr = req->addr;
            resp->len = req->len;
            resp->payload[0] = 0xff;
        } else {
            resp->dev_id = req->dev_id;
            resp->addr = req->addr;
            resp->len = req->len;
            resp->payload[0] = 15; // Example response data
        }
        return 1;
    }

private:
    int curr_press = 15;
    int target_press = 20;
    int cal_val = -4;
    int raw_press = 16;
    int valve_position = 34;


};

// Fixture class for our tests
class DeviceBlowerRegisterUpdateTest : public ::testing::Test {
protected:
    device_valve_blower_t *device;
    MockTransact mockTransact;

    void SetUp() override {
        // Initialize your device_valve_blower_t instance here
        device = create_device_handle();
        default_dev_handle(device);
        device->type = DEV_SUPA;
        device->chip_id = 0x1234;
        device->chip_id_valid = 1;


    }

    void TearDown() override {
        // Clean up after each test
        delete device;
    }
};


TEST_F(DeviceBlowerRegisterUpdateTest, UpdateRegisterSuccess) {
    // Setup the mock to simulate a successful transaction
//    EXPECT_CALL(mockTransact, transact(_, _, _))
//            .WillOnce(Return(1)); // Assuming this simulates success for one call

    // Perform the update using a lambda that matches the transact_func_t signature
    int result;
    // Set the register value to be updated
    device->target_press.update_status = WRITE_REG;
    device->target_press.write_val = 15;
    device->cur_press.update_status = REG_UPDATE;

    result = update_device_blower_register(device,
                                           [](const msg16_t *req, msg16_t *resp, uint32_t timeout) -> int {
                                               // Implement the mock behavior here
                                               if (req->type == WRITE_REQ) {
                                                   resp->dev_id = req->dev_id;
                                                   resp->addr = req->addr;
                                                   resp->len = req->len;
                                                   resp->payload[0] = 0xff;
                                               } else {
                                                   resp->dev_id = req->dev_id;
                                                   resp->addr = req->addr;
                                                   resp->len = req->len;
                                                   resp->payload[0] = 50; // Example response data
                                               }
                                               return 1;
                                           });


    ASSERT_EQ(result, 1);
    // Check that the register value was updated
    ASSERT_EQ(device->target_press.write_val, 15);
    ASSERT_EQ(device->target_press.update_status, REG_WRITTEN);
    ASSERT_EQ(device->target_press.last_updated, 0);
    ASSERT_EQ(device->cur_press.curr_value_status, REG_VALID);
    ASSERT_EQ(device->cur_press.val, 50);
    ASSERT_GT(device->cur_press.last_updated, 0);
    ASSERT_NE(device->cur_press.update_status, REG_NOT_UPDATED);

    // Add assertions to verify the status of registers...
}

// You would add more tests here to simulate and test different scenarios, e.g., transaction failure

TEST_F(DeviceBlowerRegisterUpdateTest, UpdateRegisterFail) {
//     Setup the mock to simulate a successful transaction
//    EXPECT_CALL(mockTransact, transact(_, _, _))
//            .WillOnce(Return(0)); // Assuming this simulates success for one call

    // Perform the update using a lambda that matches the transact_func_t signature
    int result;
    // Set the register value to be updated
    device->target_press.update_status = WRITE_REG;
    device->target_press.write_val = 15;
    device->cur_press.update_status = REG_UPDATE;

    result = update_device_blower_register(device,
                                           [](const msg16_t *req, msg16_t *resp, uint32_t timeout) -> int {
                                               // Implement the mock behavior here
                                               if (req->type == WRITE_REQ) {
                                                   resp->dev_id = req->dev_id;
                                                   resp->addr = req->addr;
                                                   resp->len = req->len;
                                                   resp->payload[0] = 0xff;
                                               } else {
                                                   resp->dev_id = req->dev_id;
                                                   resp->addr = req->addr;
                                                   resp->len = req->len;
                                                   resp->payload[0] = 0xff; // Example response data
                                               }
                                               return 0;
                                           });


    ASSERT_EQ(result, 1);
    // Check that the register value was not updated
    ASSERT_EQ(device->target_press.write_val, 15);
    ASSERT_EQ(device->target_press.update_status, REG_NOT_UPDATED);
    ASSERT_EQ(device->target_press.last_updated, 0);
    ASSERT_EQ(device->cur_press.curr_value_status, REG_NOT_UPDATED);
    ASSERT_NE(device->cur_press.val, 50);
    ASSERT_EQ(device->cur_press.last_updated, 0);
    ASSERT_EQ(device->cur_press.update_status, REG_NOT_UPDATED);

    // Add assertions to verify the status of registers...
}
