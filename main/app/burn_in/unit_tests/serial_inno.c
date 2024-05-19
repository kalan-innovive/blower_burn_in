#include <stdlib.h>
#include "serial_inno.h"

//
// Created by Erik Kalan on 3/17/24.
//



register_moc_list supla = DEFAULT_REGISTER_MOC_LIST;
register_moc_list suplb = DEFAULT_REGISTER_MOC_LIST;
register_moc_list exha = DEFAULT_REGISTER_MOC_LIST;
register_moc_list exhb = DEFAULT_REGISTER_MOC_LIST;


static register_moc_list *getDevice(int devid) {
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

void set_min_blowby(int devid, int val) {
    register_moc_list *dev = getDevice(devid);
    dev->min_blowby = val;
}

int get_current_pressure(int devid, unsigned *val) {
    register_moc_list *dev = getDevice(devid);

    if (dev->set_point == 0) {
        dev->cur_pres = 0 + dev->min_blowby;
        // Add some randomness to get different numbers
        dev->cur_pres -= rand() % 10;


    } else {
        dev->cur_pres += (int) (((dev->set_point - dev->cur_pres) / 2) + 2);
        dev->cur_pres = (dev->cur_pres > 2000) ? 2000 : dev->cur_pres;
    }
    *val = dev->cur_pres;
    return 1;
}

int set_pwm(int devid, unsigned val) {
    register_moc_list *dev = getDevice(devid);

    dev->cur_pres = (((int) val * 1200) / 100);
    return 1;

}

int get_target_pressure(int devid, unsigned *val) {
    register_moc_list *dev = getDevice(devid);
    *val = dev->set_point;
    return 1;

}

int set_target_pressure(int devid, unsigned val) {
    register_moc_list *dev = getDevice(devid);

    if (val == 0) {
        dev->cur_pres = 0;
    }
    dev->set_point = val;

    return 1;

}

int get_caibration(int devid, int *val) {
    register_moc_list *dev = getDevice(devid);

    *val = dev->calibration;
    return 1;
}

int get_fan_rpm(int devid, unsigned *val) {
    register_moc_list *dev = getDevice(devid);

    *val = dev->set_point * 1000;
    return 1;
}

int min_valve_pos = 750;
int max_valve_pos = 47000;
int max_pres = 2000;

int get_valve_position(int devid, unsigned *val) {
    register_moc_list *dev = getDevice(devid);
    unsigned ret;
    get_current_pressure(devid, &ret);
    if (ret < dev->min_blowby) {
        ret = 0;
    }
    ret = ret * (max_valve_pos - min_valve_pos) / (max_pres - 0) + min_valve_pos;
    *val = ret;
    return 1;

}


int get_raw_pressure(int devid, int *val) {
    register_moc_list *dev = getDevice(devid);

    *val = dev->calibration - dev->cur_pres;
    return 1;
}


int get_pwm(int devid, unsigned *val) {
    register_moc_list *dev = getDevice(devid);

    if (dev->cur_pres == 0) {
        *val = 0;
    } else {
        *val = (dev->cur_pres * 100) / 1200;
    }
    return 1;
}

int run_caibration(int devid) {
    register_moc_list *dev = getDevice(devid);
    dev->calibration = (int) ((rand() % 10) - 5);
    return 1;
}
