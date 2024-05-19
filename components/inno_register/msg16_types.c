//
// Created by Erik Klan on 1/31/24.
//

#include "msg16_types.h"


msg16_t *create_msg16(uint16_t type, uint16_t dev_id, uint16_t addr, uint16_t len, uint16_t *payload) {
    msg16_t *p;
    p = (msg16_t *) malloc(sizeof(msg16_t));
    // Asign the attributes to the msg16 struct
    p->type = type;
    p->addr = addr;
    p->dev_id = dev_id;
    p->len = len;
    // iterate through length adding payload
    if (len > MSG_16_PAYLOAD_LEN) {
        p->len = 0;
        return p;
    }
    for (int i = 0; i < len; i++) {
        p->payload[i] = payload[i];
    }

    return p;
}

int destruct_msg16(msg16_t **msg) {
    if (*msg == NULL) {
        return -1;
    }
    free(*msg);
    *msg = NULL;
    return 1;
}