/*
 * serial_inno_test.c
 *
 *  Created on: May 1, 2023
 *      Author: ekalan
 */
//#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "serial_inno_test.h"
#include "serial_inno_api_test.h"
#include "serial_inno.h"
#include <string.h>


#include "msg16.h"
#include "rack_device.h"
#include "esp_log.h"
#include "esp_err.h"

static const char* tag = "serial_inno_test";

static esp_err_t unit_test_blower_read_pack(void);
static esp_err_t unit_test_blower_read_unpack(void);

static esp_err_t insys_test_urt_rx_tx_task(void);
static esp_err_t insys_test_unit_urt_get_offset_(void);
static esp_err_t insys_test_urt_write_offset_(void);
static esp_err_t insys_test_transact(void);
static esp_err_t serial_inno_blower_comm_test(void);

static esp_err_t unit_test_rack_task_(void);
static esp_err_t system_test_transact_(void);
static esp_err_t system_test_chipID_(void);
static esp_err_t system_test_rawPres(void);


static void print_msg16(const msg16_t* msg);
static void print_msg16_transact(const msg16_t* msg_req, const msg16_t* msg_resp);
static esp_err_t comp_msg16(const msg16_t *msg_exp, const msg16_t *msg);



extern TaskHandle_t rack_task_handle;
extern TaskHandle_t uart_rx_handle;
extern QueueHandle_t uart_rx_queue;



esp_err_t serial_inno_unit_tests(void){
	esp_err_t ret;
	setup_driver();
	// Start the task for receiving
	xTaskCreate(&uart_rx_task, "uart_rx_task", 2048,
				NULL, configMAX_PRIORITIES, &uart_rx_handle);
	xTaskCreate(&rack_task, "rack_task", 2048,
					NULL, 6, &rack_task_handle);
	vTaskDelay(10000 / portTICK_PERIOD_MS);


	ret = unit_test_blower_read_pack();
	ret = unit_test_blower_read_unpack();
	ret = unit_test_rack_task_();

	vTaskDelete(rack_task_handle);
	vTaskDelete(uart_rx_handle);

	return ret;
}


esp_err_t serial_inno_system_tests(void){
	esp_err_t ret;
	setup_driver();
	// Start the task for receiving
	xTaskCreate(&uart_rx_task, "uart_rx_task", 2048,
				NULL, configMAX_PRIORITIES, &uart_rx_handle);
	xTaskCreate(&rack_task, "rack_task", 2048,
					NULL, 6, &rack_task_handle);
	vTaskDelay(1000000 / portTICK_PERIOD_MS);


//	ret = system_test_chipID_();
	ret = system_test_rawPres();
//	ret = system_test_transact_();

	vTaskDelete(rack_task_handle);
	vTaskDelete(uart_rx_handle);

	return ret;
}


esp_err_t serial_inno_in_system_tests(void){
	esp_err_t ret;
	setup_driver();
	// Start the task for receiving
	xTaskCreate(&uart_rx_task, "uart_rx_task", 2048,
				NULL, 6, &uart_rx_handle);
	xTaskCreate(&rack_task, "rack_task", 2048,
					NULL, 6, &rack_task_handle);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ret = insys_test_unit_urt_get_offset_();
	ret = insys_test_transact();
//	ret = insys_test_urt_rx_tx_task();
//	ret &= insys_test_unit_urt_get_offset_();
//	ret &= insys_test_urt_write_offset_();
//	vTaskDelete(rack_task_handle);
//	vTaskDelete(uart_rx_handle);

	return ret;
}


esp_err_t serial_inno_blower_tests(void){
	esp_err_t ret = ESP_OK;
	setup_driver();
	// Start the task for receiving
	xTaskCreate(&uart_rx_task, "uart_rx_task", 6*1024,
				NULL, 1, &uart_rx_handle);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	valve_function_test(NULL);
//	xTaskCreate(&rack_task, "rack_task", 2048,
//					NULL, 6, &rack_task_handle);

//	ret = serial_inno_blower_api_tests();

//	ret = serial_inno_blower_comm_test();
//	ret = insys_test_urt_rx_tx_task();
//	ret &= insys_test_unit_urt_get_offset_();
//	ret &= insys_test_urt_write_offset_();
//	vTaskDelete(rack_task_handle);
//	vTaskDelete(uart_rx_handle);

	return ret;
}




/*
 * Unit Test
 * Run with other unit tests
 * Creates a blower read msg16 package
 * Packs the message into a buffer and compares the message to
 * known values
 * Passing: message length is correct and bytes are the same
 */
static esp_err_t unit_test_blower_read_pack(void){
	esp_err_t ret = ESP_OK;
	// Create the expected test results
	size_t exp_len = 10;
	char buf_expected[40]= {'\0'};
	sprintf(buf_expected, "%s", "7e:01:20:03:06:10:01:3b:00:7e");

	// MSG16 to pack into datastream
	msg16_t msg = {
		.type = READ_REQ,
		.dev_id = DEV_SUPA,
		.addr = REG_MODBUS_ADDR,
		.len = 1,
		.payload[0] = 0,
	};

	// Create buffer for message pack
	uint8_t msg_buf[20];

	//TEST:
	//Try packing the message
	size_t len_;
	size_t len= pack_msg16(&msg, msg_buf, &len_);
	if (len!=len_){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Referenced length: %d does not equal returned length: %d", len_, len);

	}
	if (len != exp_len){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected length: %d does not equal returned length: %d", exp_len, len);

	}
	int pos = 0;
	char buf[40]= {'\0'};


		// Print out the message and compare the values
	for (int i = 0; i<len;i++){
		pos += sprintf(&buf[pos], "%02x:",msg_buf[i] );
	}
	ESP_LOGI(tag, "ACCT BUF: 0X7e:01:20:03:06:10:01:3b:00:7e");
	ESP_LOGI(tag, "RESP BUF: 0X%s", buf);
	if (strcmp(buf, buf_expected) != 0){
		ret = (ret == ESP_FAIL) ? ESP_FAIL: ESP_OK;
		ESP_LOGW(tag, "Expected Buffer string does not equal packed");
	}

	return ret;
}

/*
 * Unit Test
 * Run with other unit tests
 * Creates a blower read response msg16 package
 * Packs the message into a buffer and compares the message to
 * known values
 * Passing: message length is correct and bytes are the same
 */
static esp_err_t unit_test_blower_read_unpack(void){
	esp_err_t ret = ESP_OK;
	size_t len = 9;
	uint8_t rec_data[10] = {0x7e,0xff,0xa0,0x02,0x00,0x00,0xa1,0x01,0x7e,0x00};


	msg16_t msg = {
		.type = READ_RESP,
		.dev_id = DEV_UNDEF,
		.addr = REG_STATUS,
		.len = 1,
		.payload[0] = 0x00,
	};
	//TEST:
	//Try unpacking the message
	msg16_t resp_msg;
	size_t resp_len = unpack_msg16(rec_data, len, &resp_msg);

	ESP_LOGI(tag, "Testing Buffer BUF: 0x7e:ff:a0:02:00:00:a1:01:7e");
	print_msg16_transact(&msg, &resp_msg);

	if (len!=resp_len){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Referenced length: %d does not equal returned length: %d", resp_len, len);
		comp_msg16(&msg, &resp_msg);
	}

	return ret;
}



/*
 * In-system Test: Test the hardware configuration
 * Test requires uart0rx and uart0tx to be linked. hardware to be connected
 * Creates an rx msg buffer sends the message and compares the response
 * Packs the message into a buffer and compares the message to
 * known values
 * Passing: if the buffers are the same
 */
static esp_err_t insys_test_urt_rx_tx_task(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	setup_driver();

	/*TODO pack the message into a msg16_t and send it*/
	uint8_t rec_data[10] = {0x7e,0xff,0xa0,0x02,0x00,0x00,0xa1,0x01,0x7e,0x00};

	// Start the task for receiving
	xTaskCreate(&uart_rx_task, "uart_rx_task", 2048,
				NULL, 6, NULL);

	uart_tx_task(rec_data, 9);
	msg16_t* tx_msg = malloc(sizeof(msg16_t));

	// Wait for message print out response
	if (xQueueReceive(uart_rx_queue,  &tx_msg,
			portMAX_DELAY)) {
		ESP_LOGI(tag, "Received Msg");
		print_msg16(tx_msg);
		ret = ESP_OK;
	} else {
		ESP_LOGW(tag, "Transaction Timed out");
		ret = ESP_FAIL;


	}
	vTaskDelay(10000 / portTICK_PERIOD_MS);
	free(tx_msg);


	/*TODO Get message from Queue*/
	// Need the callback function
	return ret;
}


/*
 * In-system Test: Test Transaction Time
 * Test requires uart0rx and uart0tx to be linked. hardware to be connected
 * Creates an tx msg buffer  response and sends it as a request,
 * The buffer is received tnd the total transact time is decreased until it fails
 * Passing: if the transaction ever works
 */
static esp_err_t insys_test_transact(void){
	// Create the messages
	msg16_t msg_req = {
		.type = READ_RESP,
		.dev_id = DEV_SUPA,
		.addr = REG_RAW_PRESS,
		.len = 1,
		.payload[0] = 52,
	};
	msg16_t msg_res;

//	int tmp = 0;
	int trans_result = 0;
	TickType_t timeout = 100;
	ESP_LOGI(tag, "Testing transact_read timeout: %d", (int) timeout);

	trans_result = transact_read(&msg_req, &msg_res, timeout);
	ESP_LOGI(tag, "Result transact test: %d", trans_result);
	return (trans_result == 1) ? ESP_OK: ESP_FAIL;

}

/*
 * Testing blower communication Turnaround time
 * Test requires Blower to be connected and runs requests of different length and tests the
 * minimum time before transaction times out
 * Creates an tx msg buffer  response and sends it as a request,
 * The buffer is received and the total transact time is decreased until it fails
 * Starts at 100 ms
 * Passing: Transaction of up to 24 length and less than 5ms
 *
 */
static esp_err_t serial_inno_blower_comm_test(void){
	// Create the messages
	msg16_t msg_req = {
		.type = READ_REQ,
		.dev_id = DEV_SUPA,
		.addr = REG_STATUS,
		.len = 1,
		.payload[0] = 0,
	};
	msg16_t msg_res;
	int trans_result = 0;
	TickType_t timeout = 0;
	TickType_t testing_period = 50;
	TickType_t passsing_period = 6;
	TickType_t max_timeout = testing_period;
	vTaskDelay(5000 / portTICK_PERIOD_MS);


	for(TickType_t j=testing_period; j >= 1 ; j--){

		timeout = j;
		ESP_LOGI(tag, "__Testing transact_read period: %d", (int) j);

		for(int i =1; i<=24; i++) {
			trans_result = 0;
			msg_req.len = i;
			msg_res.len = 0;
			vTaskDelay(0 / portTICK_PERIOD_MS);

			trans_result = transact_read(&msg_req, &msg_res, timeout);
			vTaskDelay(0 / portTICK_PERIOD_MS);


			if (trans_result != 1) {
				ESP_LOGW(tag, "Transaction failed on length: %d, timeout=%d, result %d ", msg_req.len, (int)timeout, trans_result);
				vTaskDelay(5000 / portTICK_PERIOD_MS);
				break;
			}
			else if(msg_req.len != msg_res.len){
				ESP_LOGE(tag, "Transaction Received incorrect frame length: %d, timeout=%d, result %d ", (int)msg_res.len, (int)timeout, trans_result);
				vTaskDelay(5000 / portTICK_PERIOD_MS);
				break;
			}
			else if (trans_result == 1){
				ESP_LOGI(tag, "_Test Length=%d  : timeout=%d ms   : errrormsg=%d",i, (int)timeout, trans_result);
			}
			else {
				ESP_LOGW(tag, "Logic Error: length: %d, timeout=%d, result %d ", i, (int)timeout, trans_result);
				vTaskDelay(5000 / portTICK_PERIOD_MS);
			}
		}


		max_timeout = (max_timeout > timeout) ? max_timeout : timeout;
		ESP_LOGI(tag, "_____Result transact test: Length=%d   : timeout=%d ms   : errrormsg=%d",msg_req.len, (int)timeout, trans_result);
		vTaskDelay(3000 / portTICK_PERIOD_MS);
	}

	ESP_LOGI(tag, "_______Result transact test: %d", (int)max_timeout);
	return (timeout <= passsing_period ) ? ESP_OK: ESP_FAIL;

}


/*
 * In-system Test: Test the hardware configuration and methods for encoding and decoding msg16
 * Test requires uart0rx and uart0tx to be linked. hardware to be connected
 * Creates an tx msg buffer sends the message and compares the response
 * known values
 * Passing: if the buffers are the same
 */
static esp_err_t insys_test_unit_urt_get_offset_(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	setup_driver();
	uint16_t v = (uint16_t) -4;
	ESP_LOGI(tag, "msg value 0x%02x", v);

	/*TODO pack the message into a msg16_t and send it*/
	msg16_t msg = {
		.type = READ_RESP,
		.dev_id = DEV_SUPA,
		.addr = REG_RAW_PRESS,
		.len = 1,
		.payload[0] = v,
	};
	uint8_t rec_data[24];
	size_t msg_len;
	size_t len = pack_msg16(&msg, rec_data, &msg_len);
	if (len==0){
		ret = ESP_FAIL;
//		return ret;
	}

	uart_tx_task(rec_data, len);
	msg16_t* tx_msg = malloc(sizeof(msg16_t));

	// Wait for message print out response
	if (xQueueReceive(uart_rx_queue,  &tx_msg,
			portMAX_DELAY)) {
		ESP_LOGI(tag, "Received Offset");
		print_msg16(tx_msg);
		ret = ESP_OK;
	}
	free(tx_msg);
	vTaskDelay(10000 / portTICK_PERIOD_MS);


	/*TODO Get message from Queue*/
	// Need the callback function
	return ret;
}



/*
 * In-system Test: Test the hardware configuration and methods for encoding and decoding msg16
 * Test requires uart0rx and uart0tx to be linked. hardware to be connected
 * Sends a msg16 data packet from tx to rx and compares the response
 * known values
 * Passing: if the structs are the same
 */
static esp_err_t insys_test_urt_write_offset_(void){
	// Setup
	esp_err_t ret = ESP_OK;

	uint16_t v = (uint16_t) 1000;
	ESP_LOGI(tag, "Testing write offeset msg value 0x%02x", v);

	// Test
	// write offset message should return same message with value v
	msg16_t msg = {
		.type = WRITE_REQ,
		.dev_id = DEV_SUPA,
		.addr = REG_TARG_PRESS,
		.len = 1,
		.payload[0] = v,
	};
	uint8_t rec_data[24];
	size_t msg_len;
	size_t len = pack_msg16(&msg, rec_data, &msg_len);
	if (len==0){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Buffer length: %d equal 0", len);
	}


	// Send the message
	uart_tx_task(rec_data, len);
	msg16_t* tx_msg = malloc(sizeof(msg16_t));

	// Wait for message print out response
	if (xQueueReceive(uart_rx_queue,  &tx_msg,
			portMAX_DELAY)) {
		ESP_LOGI(tag, "Received Msg");
		print_msg16(tx_msg);
		ret = (ret==ESP_FAIL)? ret: comp_msg16(&msg, tx_msg);
	}
	free(tx_msg);
	vTaskDelay(10000 / portTICK_PERIOD_MS);

	return ret;
}

/*
 * Unit Test: testst the Rack Device task can interperate a message and respond
 * This is an unit test for code usage
 * Test should be run with other unit tests
 * Create a rack request msg16
 * add to the rack_queue and retrieve response
 */
static esp_err_t unit_test_rack_task_(void){
	// Setup
	esp_err_t ret = ESP_OK;

	ESP_LOGI(tag, "____Testing Rack Device Handle MSG____");
	// Rack read request
	msg16_t msg = {
		.type = READ_REQ,
		.dev_id = DEV_RACK,
		.addr = INFO_SIDES,
		.len = 1,
		.payload[0] = 0x00,
	};
	msg16_t* msg_resp = malloc(sizeof(msg16_t));

	setup_rack_dev_default();
	handle_rack_req(&msg, msg_resp);

	if (msg_resp->len == 0){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Could not handle request received default len:%d", msg_resp->len);
	}
	if (msg_resp->payload[0] != 2){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Msg response should default to 2 actual:%d", msg_resp->payload[0]);
	}

	free(msg_resp);
	vTaskDelay(10000 / portTICK_PERIOD_MS);

	return ret;
}


/*
 * Test requires uart0rx and uart0tx to be connected to rack
 * This is a system test
 * Test should be run with other system tests configurations
 * TODO: send write and read the result
 */
static esp_err_t system_test_transact_(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	uint16_t v = (uint16_t) 1000;
	ESP_LOGI(tag, "____Testing Blower System Transact ____");

	ESP_LOGI(tag, "msg value 0x%02x", v);

	/*TODO pack the message into a msg16_t and send it*/
	msg16_t msg_req = {
		.type = READ_REQ,
		.dev_id = DEV_EXB ,
		.addr = REG_RAW_PRESS,
		.len = 1,
	};



	// Iterate through all device id
	int dev_id_len = 5;
	int dev_id[5] = {DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB, DEV_UNDEF};
	for ( int i=0; i<dev_id_len;i++){
		// Send transaction
		msg16_t msg_resp;
		msg_resp.len = 0;

		// Set the id
		msg_req.dev_id = dev_id[i];
		int suc = transact_read(&msg_req,&msg_resp, 30);


		if (suc<1){
			ret = ESP_FAIL;
			ESP_LOGW(tag, "Testing ID:%x Error Success code 0x%02x", dev_id[i], suc);
		} else{
			ESP_LOGI(tag, "Transact Received Msg");
			print_msg16_transact(&msg_req, &msg_resp);
			ret = ESP_OK;

			break;
		}
		vTaskDelay(10000 / portTICK_PERIOD_MS);

	}

	// Wait for message print out response
//
//	ESP_LOGI(tag, "Transact Received Msg");
//	print_msg16_transact(&msg_req, &msg_resp);

	vTaskDelay(10000 / portTICK_PERIOD_MS);


	return ret;
}

/*
 * Test requires uart0rx and uart0tx to be connected to rack
 * This is a system test
 * Test should be run with other system tests configurations
 * TODO: send write and read the result
 */
static esp_err_t system_test_chipID_(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	ESP_LOGI(tag, "____Testing Blower ChipID ____");

	unsigned chipid = 0;
	int suc = 0;

	suc = get_chipid(DEV_SUPA, &chipid);

	if (suc <1){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Testing ID:%x ChipID Error Success code %d", DEV_SUPA, suc);
	} else {
		ESP_LOGI(tag, "Chip ID Received %u", chipid);
		ret = ESP_OK;
	}

	vTaskDelay(10000 / portTICK_PERIOD_MS);


	return ret;
}

/*
 * Test requires uart0rx and uart0tx to be connected to rack
 * This is a system test to test turning on the rack
 * To complete test turn the rack off start test and turn rack on for a second and then back off
 * Test should be run with other system tests configurations
 * TODO: send write and read the result
 */
static esp_err_t system_test_available_dev(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	ESP_LOGI(tag, "____Testing Rack Device Detection ____");

	unsigned chipid = 0;
	int suc = 0;

	/* TODO set a timer to test the on off loop*/
	bool rack_on = false;
	int rack_state = 0;// Rack state
	int num_dev = 4;
	int num_avail = 0;
	int dev_id[4] = {DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB};
	unsigned dev_chipid[4] = {0};

	// Rack state even is off odd is on
	// 0=not on
	// 1=on
	// 2=off
	while (rack_state<1){
		// Value that holds available devices
		// Iterate throught the devices and check if they respond
		rack_on = false;
		for (int i=0;i<num_dev; i++){
			// Send a message and check for response
			suc = get_chipid(dev_id[i], &chipid);
			if (suc>=1){
				ESP_LOGI(tag, "Chip ID Received %u for Dev:%d", chipid, dev_id[i]);
				dev_chipid[i] = chipid;
				num_avail +=1;
				rack_on = true;
			}
		}
//		for (int i=0;i<num_dev; i++){
//			num_avail += (dev_chipid )
//		}
		// If the rack is on and the state is even increment state
		if (rack_on && (rack_state %2) ==0){
			rack_state +=1;
			//report chipid
		}
		// If the rack is off and the state is odd increment state
		else if (!rack_on && (rack_state %2) ==1){
			rack_state +=1;
			num_avail = 0;
		}
		// Delay for a second try again
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	return ESP_OK;
}

/*
 * Test requires uart0rx and uart0tx to be connected to rack
 * This is a system test
 * Test should be run with other system tests configurations
 * TODO: send write and read the result
 */
static esp_err_t system_test_rawPres(void){
	// Setup
	esp_err_t ret;
	ret = ESP_OK;
	ESP_LOGI(tag, "____Testing Blower Raw Pressure____");

	int raw = 0;
	int suc = 0;

	suc = get_raw_pressure(DEV_SUPA, &raw);

	if (suc <1){
		ret = ESP_FAIL;
		ESP_LOGW(tag, "Testing ID:%x raw Pres Error Success code %d", DEV_SUPA, suc);
	} else {
		ESP_LOGI(tag, "Raw Pressure Received %d", raw);
		ret = ESP_OK;
	}

	vTaskDelay(10000 / portTICK_PERIOD_MS);


	return ret;
}



static void print_msg16(const msg16_t* msg){
	ESP_LOGI(tag, "Device ID:%d | ", msg->dev_id);
	ESP_LOGI(tag, "MSG Type :%02x | ", msg->type);
	ESP_LOGI(tag, "MSG len  :%d |",  msg->len);
	for (int i = 0; i<(msg->len%10);i++){
		ESP_LOGI(tag, "%d   Val :%d |", i, (int16_t)msg->payload[i]);
	}
}

static void print_msg16_transact(const msg16_t* msg_req, const msg16_t* msg_resp){
	ESP_LOGI(tag, "Device ID:%d | %d", msg_req->dev_id,msg_resp->dev_id);
	ESP_LOGI(tag, "MSG Type :%d | %d", msg_req->type, msg_resp->type);
	ESP_LOGI(tag, "MSG len  :%d | %d", msg_req->len, msg_resp->len);

	for (int i = 0; i<msg_resp->len%14;i++){
		ESP_LOGI(tag, "%d   Val :%d | %d", i, msg_req->payload[i], msg_resp->payload[i]);
	}

}

static esp_err_t comp_msg16(const msg16_t *msg_exp, const msg16_t *msg){
	esp_err_t ret = ESP_OK;

	if (msg_exp->type != msg->type){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected Type: %d does not equal returned Type: %d", msg_exp->type , msg->type);

	}
	if (msg_exp->dev_id != msg->dev_id){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected dev_id: %d does not equal returned dev_id: %d", msg_exp->dev_id , msg->dev_id);

	}
	if (msg_exp->addr != msg->addr){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected addr: %d does not equal returned addr: %d", msg_exp->addr , msg->addr);
	}
	if (msg_exp->len != msg->len){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected len: %d does not equal returned len: %d", msg_exp->len , msg->len);
	}
	if (msg_exp->payload[0] != msg->payload[0]){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Expected val: %d does not equal returned val: %d", msg_exp->payload[0] , msg->payload[0]);
	}
	return ret;
}


