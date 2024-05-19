//
// Created by Erik Klan on 5/14/24.
//

#include "MockSerialInno.h"
//#include "serial_inno.h"

MockSerialInno *mockAPI = nullptr;


MockSerialInno::register_moc_list *MockSerialInno::getDevice(int devid) {
    // Get switch on the device id to asign the correct moc
    switch (devid) {
        case 0x1:
            return &supla;
        case 0x3:
            return &suplb;
        case 0x21:
            return &exha;
        case 0x23:
            return &exhb;

    }
}

extern "C" {


int get_current_pressure(int devid, unsigned *val) {

    int ret = mockAPI->get_current_pressure(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);
    if (dev->set_point == 0) {
        dev->cur_pres = 0 + dev->min_blowby;
        // Add some randomness to get different numbers
        dev->cur_pres -= rand() % 10;


    } else {
        dev->cur_pres += (int) (((dev->set_point - dev->cur_pres) / 2) + 2);
        dev->cur_pres = (dev->cur_pres > 2000) ? 2000 : dev->cur_pres;
    }

    if (*val == 0xFE) {
        *val = dev->cur_pres;
    }

    return ret;
}

int get_blower_type(int devid, unsigned *blower_type) {
    return mockAPI->get_blower_type(devid, blower_type);
}

int set_blower_type(int devid, int val) {
    return mockAPI->set_blower_type(devid, val);
}

int min_valve_pos = 750;
int max_valve_pos = 47000;
int max_pres = 2000;

int get_valve_position(int devid, unsigned *val) {
    int ret = mockAPI->get_valve_position(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);
    unsigned curr_pres;
    get_current_pressure(devid, &curr_pres);
    if (curr_pres < dev->min_blowby) {
        curr_pres = 0;
    }
    unsigned pos = curr_pres * (max_valve_pos - min_valve_pos) / (max_pres - 0) + min_valve_pos;
    if (*val == 0xFE) {
        *val = pos;
    }

    return ret;
}

int get_raw_pressure(int devid, int *val) {
    int ret = mockAPI->get_raw_pressure(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    int tmp_val = dev->calibration - dev->cur_pres;

    if (*val == 0xFE) {
        *val = tmp_val;
    }

    return ret;
}

int get_caibration(int devid, int *val) {
    int ret = mockAPI->get_caibration(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    int tmp_val = dev->calibration;

    if (*val == 0xFE) {
        *val = tmp_val;
    }

    return ret;
}

int run_caibration(int devid) {
    int ret = mockAPI->run_caibration(devid);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    dev->calibration = (int) ((rand() % 10) - 5);

    return ret;
}

int get_target_pressure(int devid, unsigned *val) {
    int ret = mockAPI->get_target_pressure(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    unsigned tmp_val = dev->set_point;

    if (*val == 0xFE) {
        *val = tmp_val;
    }

    return ret;
}

int set_target_pressure(int devid, unsigned val) {
    int ret = mockAPI->set_target_pressure(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    if (val == 0) {
        dev->cur_pres = 0;
    }
    dev->set_point = val;
    return ret;
}

int get_fan_rpm(int devid, unsigned *val) {
    int ret = mockAPI->get_fan_rpm(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    unsigned tmp_val = dev->set_point * 1000;


    if (*val == 0xFE) {
        *val = tmp_val;
    }

    return ret;
}

int get_pwm(int devid, unsigned *val) {
    int ret = mockAPI->get_pwm(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    int tmp_val;
    if (dev->cur_pres == 0) {
        tmp_val = 0;
    } else {
        tmp_val = (dev->cur_pres * 100) / 1200;
    }

    if (*val == 0xFE) {
        *val = tmp_val;
    }

    return ret;
}

int set_pwm(int devid, unsigned val) {
    int ret = mockAPI->set_pwm(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    dev->cur_pres = (((int) val * 1200) / 100);

    return ret;
}

void set_min_blowby(int devid, int val) {
    mockAPI->set_min_blowby(devid, val);
    MockSerialInno::register_moc_list *dev = mockAPI->getDevice(devid);

    dev->min_blowby = val;

}
}