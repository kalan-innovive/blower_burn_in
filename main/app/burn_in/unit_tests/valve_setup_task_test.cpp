//
// Created by Erik Klan on 5/9/24.
//

//#include "valve_setup_task_test.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "serial_inno.h"
#include "valve_setup_task.h"
#include "burnin_types.h"
#ifdef __cplusplus
}
#endif

#include <gmock/gmock.h>
//#include <gmock/gmock-matchers.h>
#include "gtest/gtest.h"
#include "valve_setup_task_test.h"
#include "MockSerialInno.h"


class BurnInSetupTest : public ::testing::Test {
protected:
    calibration_conf_t *cal_conf;

    virtual void SetUp() {
        mockAPI = new MockSerialInno();

        cal_conf = create_calibration_config();

        cal_conf->vdev = create_device_handle();
        cal_conf->cdev = create_device_handle();
        cal_conf->vdev->type = DEV_SUPA;
        cal_conf->cdev->type = DEV_EXHA;
        cal_conf->vdev->blower_type = VALVE;
        cal_conf->cdev->blower_type = CONTROL;
        cal_conf->vdev->chip_id = 1234567;
        cal_conf->cdev->chip_id = 7654321;

        cal_conf->vdev->chip_id_valid = 1;
        cal_conf->cdev->chip_id_valid = 1;
        cal_conf->id = DEV_SUPA;
        cal_conf->contr_id = DEV_EXHA;
        ON_CALL(*mockAPI, set_target_pressure(testing::_, testing::_))
                .WillByDefault(testing::Return(1));
        ON_CALL(*mockAPI, get_current_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_valve_position(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_raw_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_caibration(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, run_caibration(testing::_))
                .WillByDefault(testing::Return(1));
        ON_CALL(*mockAPI, get_target_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_fan_rpm(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_pwm(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));


    }

    virtual void TearDown() {
        destruct_calibration_config(cal_conf);
    }
};

TEST_F(BurnInSetupTest, CalibrationConfigTime) {
    cal_conf->time_out = 1;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_calibration_task), (void *) cal_conf);
    pthread_join(thread_id, NULL);
//    sleep(1);
    EXPECT_EQ(cal_conf->state, cal_failed);
    EXPECT_EQ(cal_conf->burnin_state, burnin_test_failed);
    EXPECT_GT(cal_conf->stop, cal_conf->start);
    EXPECT_EQ(cal_conf->iterations, 0);
    EXPECT_EQ(cal_conf->max_press, CAL_CONF_MAX_PRESS);
    EXPECT_EQ(cal_conf->cal_val, CAL_CONF_DEFAULT_VAL);
}


TEST_F(BurnInSetupTest, CalibrationConfigStart) {
    int actual;
    // Create a thread to run function
    cal_conf->time_out = 30000;
    cal_conf->cal_val = -14;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_calibration_task), (void *) cal_conf);
    pthread_join(thread_id, NULL);

    EXPECT_EQ(cal_conf->state, cal_success);
    EXPECT_EQ(cal_conf->burnin_state, burnin_test_passed);
    EXPECT_GT(cal_conf->stop, cal_conf->start);
    EXPECT_GE(cal_conf->iterations, 1);
    EXPECT_NE(cal_conf->cal_val, -14);

}

class BurnInBlowByTest : public ::testing::Test {
protected:
    blowby_conf_t *blowbyConf;

    virtual void SetUp() {

        mockAPI = new MockSerialInno();
        blowbyConf = create_blowby_conf();
        blowbyConf->vdev = create_device_handle();
        blowbyConf->cdev = create_device_handle();
        blowbyConf->vdev->type = DEV_SUPA;
        blowbyConf->cdev->type = DEV_EXHA;
        blowbyConf->vdev->blower_type = VALVE;
        blowbyConf->cdev->blower_type = CONTROL;
        blowbyConf->vdev->chip_id = 1234567;
        blowbyConf->cdev->chip_id = 7654321;
        blowbyConf->vdev->chip_id_valid = 1;
        blowbyConf->cdev->chip_id_valid = 1;
        blowbyConf->id = DEV_SUPA;
        blowbyConf->contr_id = DEV_EXHA;
        blowbyConf->time_out = 35;
        blowbyConf->loop_time = 1;
        set_min_blowby(DEV_SUPA, 30);
        set_min_blowby(DEV_EXHA, 30);

        ON_CALL(*mockAPI, set_target_pressure(testing::_, testing::_))
                .WillByDefault(testing::Return(1));
        ON_CALL(*mockAPI, get_current_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_valve_position(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_raw_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_caibration(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, run_caibration(testing::_))
                .WillByDefault(testing::Return(1));
        ON_CALL(*mockAPI, get_target_pressure(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
        ON_CALL(*mockAPI, get_fan_rpm(testing::_, testing::_))
                .WillByDefault(testing::DoAll(testing::SetArgPointee<1>(0xFE), testing::Return(1)));
    }

    virtual void TearDown() {
        destruct_blowby_conf(blowbyConf);
        delete mockAPI;
    }
};

TEST_F(BurnInBlowByTest, BlowByConfigTimeOut) {

    // Create a thread to run function
    blowbyConf->time_out = 1;
    blowbyConf->loop_time = 1001;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_blowby_task), (void *) blowbyConf);
    pthread_join(thread_id, NULL);


    EXPECT_EQ(blowbyConf->state, blowby_valve_failed);
    EXPECT_GT(blowbyConf->stop, blowbyConf->start);
    EXPECT_EQ(blowbyConf->iterations, 0);
    EXPECT_EQ(blowbyConf->blow_by_val, 0);
    EXPECT_EQ(blowbyConf->max_press_val, 0);
}

TEST_F(BurnInBlowByTest, BlowByPassing) {

    // Create a thread to run function
    blowbyConf->time_out = 50;
    blowbyConf->loop_time = 1;
    set_min_blowby(DEV_SUPA, 20);


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_blowby_task), (void *) blowbyConf);
    pthread_join(thread_id, NULL);

    EXPECT_EQ(blowbyConf->state, blowby_valve_passed);
    EXPECT_GT(blowbyConf->stop, blowbyConf->start);
    EXPECT_GT(blowbyConf->iterations, 8);
    EXPECT_LT(blowbyConf->blow_by_val, blowbyConf->max_blowby);
}


TEST_F(BurnInBlowByTest, BlowByFailing) {

    // Create a thread to run function
    blowbyConf->time_out = 100;
    blowbyConf->loop_time = 1;
    set_min_blowby(DEV_SUPA, 50);


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_blowby_task), (void *) blowbyConf);
    pthread_join(thread_id, NULL);

    EXPECT_EQ(blowbyConf->state, blowby_valve_failed);
    EXPECT_GT(blowbyConf->stop, blowbyConf->start);
    EXPECT_GT(blowbyConf->iterations, 9);
    EXPECT_GT(blowbyConf->blow_by_val, BLOWBY_CONF_DEF_MAX_BLOWBY);
}


TEST_F(BurnInBlowByTest, BlowByStateInit) {

    // Create a thread to run function
    blowbyConf->time_out = 20;
    blowbyConf->loop_time = 5;
    set_min_blowby(DEV_SUPA, 40);


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(valve_blowby_task), (void *) blowbyConf);
    while (blowbyConf->state != blowby_init) {
        usleep(100);
    }
    EXPECT_EQ(blowbyConf->state, blowby_init);
    EXPECT_EQ(blowbyConf->blow_by_val, 0);
    pthread_join(thread_id, NULL);


}

TEST_F(BurnInBlowByTest, CommunicationErrors) {
    // Arrange
    set_min_blowby(DEV_SUPA, 50);
    set_min_blowby(DEV_EXHA, 50);
    blowbyConf->time_out = 30;
    blowbyConf->loop_time = 1;

    EXPECT_CALL(*mockAPI, set_target_pressure(1, 0))
            .WillRepeatedly(testing::Return(0));

    EXPECT_CALL(*mockAPI, set_target_pressure(blowbyConf->contr_id, blowbyConf->control_pressure))
            .WillRepeatedly(testing::Return(0));

    // Add expectations for other calls as needed

    // Act
    valve_blowby_task((void *) blowbyConf);

    // Assert
    // Verify the final state or any other expected outcome
    EXPECT_EQ(blowbyConf->state, blowby_valve_failed);
    EXPECT_GT(blowbyConf->iterations, 1);
    EXPECT_EQ(blowbyConf->max_press_val, 0);
    EXPECT_EQ(blowbyConf->blow_by_val, 0);
    EXPECT_EQ(*blowbyConf->records->status, 0);
    EXPECT_EQ(blowbyConf->records->index, -1);
}


TEST_F(BurnInBlowByTest, ControlPresNotReached) {
    EXPECT_CALL(*mockAPI, get_current_pressure(3, testing::_))
            .WillRepeatedly(
                    testing::DoAll(testing::SetArgPointee<1>(blowbyConf->control_pressure - 1), testing::Return(1)));

    valve_blowby_task((void *) blowbyConf);

    // Verify the final state or any other expected outcome
    EXPECT_EQ(blowbyConf->state, blowby_valve_failed);
    EXPECT_GT(blowbyConf->iterations, 1);
    EXPECT_EQ(blowbyConf->max_press_val, 0);
    EXPECT_EQ(blowbyConf->blow_by_val, 0);
    EXPECT_EQ(*blowbyConf->records->status, 0);
    EXPECT_EQ(blowbyConf->records->index, -1);
}

TEST_F(BurnInBlowByTest, BlowbyValveWaitRecordsTest) {

    EXPECT_CALL(*mockAPI, get_current_pressure(1, testing::_))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(10), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(9), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(20), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(blowbyConf->max_blowby + 1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(blowbyConf->max_blowby), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)))
            .WillOnce(testing::DoAll(testing::SetArgPointee<1>(1), testing::Return(1)));
    EXPECT_CALL(*mockAPI, get_current_pressure(3, testing::_))
            .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(1780), testing::Return(1)));


    valve_blowby_task((void *) blowbyConf);

    // Verify the final state or any other expected outcome
    EXPECT_EQ(blowbyConf->state, blowby_valve_failed);
    EXPECT_GT(blowbyConf->iterations, 12);
    EXPECT_EQ(blowbyConf->max_press_val, 0);
    EXPECT_EQ(blowbyConf->blow_by_val, blowbyConf->max_blowby + 1);
    EXPECT_EQ(blowbyConf->records->status[4], 0);
    EXPECT_EQ(blowbyConf->records->status[3], 1);
    EXPECT_EQ(blowbyConf->records->index, 9);
}