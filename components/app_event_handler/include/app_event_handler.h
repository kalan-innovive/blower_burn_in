
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
	MSG16_REQUEST,
	SETTINGS_REQUEST,
	DB_REQUEST,
};


typedef struct {
    uint16_t type;
    uint16_t dev_id;
    uint16_t addr;
    uint16_t len;
    uint16_t* msg_val;
} msg16_t;



typedef struct {
    int msg_id;
    void (*respons_cb)(void *result);
    void* request;
} app_request_t;

typedef struct {
    char* json_msg;
    int id;
    void (*respons_cb)(app_request_t *result);
} queue_task_t;


void event_handler_task(void* pvParams);

#ifdef __cplusplus
}
#endif
