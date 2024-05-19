//
// Created by Erik Klan on 5/14/24.
//

#ifndef UNIT_TESTS_MOCKSERIALINNO_H
#define UNIT_TESTS_MOCKSERIALINNO_H

#include <gmock/gmock.h>

#define DEFAULT_REGISTER_MOC_LIST {0, 0, 0, 0, 1}


class MockSerialInno {

public:
    typedef struct {
        int cur_pres;
        unsigned set_point;
        int calibration;
        int min_blowby;
        int response;
    } register_moc_list;


    register_moc_list *getDevice(int devid);

public:
    MOCK_METHOD(int, get_current_pressure, (int devid, unsigned * val), ());
    MOCK_METHOD(int, get_blower_type, (int devid, unsigned * blower_type), ());
    MOCK_METHOD(int, set_blower_type, (int devid, int val), ());
    MOCK_METHOD(int, get_valve_position, (int devid, unsigned * val), ());
    MOCK_METHOD(int, get_raw_pressure, (int devid, int * val), ());
    MOCK_METHOD(int, get_caibration, (int devid, int * val), ());
    MOCK_METHOD(int, run_caibration, (int devid), ());
    MOCK_METHOD(int, get_target_pressure, (int devid, unsigned * val), ());
    MOCK_METHOD(int, set_target_pressure, (int devid, unsigned val), ());
    MOCK_METHOD(int, get_fan_rpm, (int devid, unsigned * val), ());
    MOCK_METHOD(int, get_pwm, (int devid, unsigned * val), ());
    MOCK_METHOD(int, set_pwm, (int devid, unsigned val), ());
    MOCK_METHOD(void, set_min_blowby, (int devid, int val), ());

    register_moc_list supla = DEFAULT_REGISTER_MOC_LIST;
    register_moc_list suplb = DEFAULT_REGISTER_MOC_LIST;
    register_moc_list exha = DEFAULT_REGISTER_MOC_LIST;
    register_moc_list exhb = DEFAULT_REGISTER_MOC_LIST;
};

extern MockSerialInno *mockAPI;


#endif //UNIT_TESTS_MOCKSERIALINNO_H
