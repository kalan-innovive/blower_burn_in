//
// Created by Erik Klan on 1/31/24.
//
#include "gtest/gtest.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "msg16_types.h"
#ifdef __cplusplus
}
#endif


TEST(CreateMsg16Test, ExpectedValues) {
    uint16_t pay[] = {6};
    msg16_t *p_actual;
    p_actual = create_msg16(1, 1, 1, 1, pay);
    msg16_t p_expected = {.type = 1, .dev_id = 1, .addr = 1, .len = 1, .payload = {6}};
    ASSERT_EQ(p_expected.type, p_actual->type);
    ASSERT_EQ(p_expected.addr, p_actual->addr);
    ASSERT_EQ(p_expected.dev_id, p_actual->dev_id);
    ASSERT_EQ(p_expected.len, p_actual->len);
    free(p_actual);

}


TEST(CreateMsg16Test, PayloadCopiedCorrectly) {
    uint16_t testPayload[] = {1, 2, 3, 4, 5};
    uint16_t len = sizeof(testPayload) / sizeof(testPayload[0]);

    // Create the message
    msg16_t *msg = create_msg16(0x01, 0x02, 0x03, len, testPayload);

    // Check that the msg16 structure's fields are set correctly
    ASSERT_NE(msg, nullptr);
    EXPECT_EQ(msg->type, 0x01);
    EXPECT_EQ(msg->dev_id, 0x02);
    EXPECT_EQ(msg->addr, 0x03);
    EXPECT_EQ(msg->len, len);

    // Compare the payload
    EXPECT_EQ(0, memcmp(msg->payload, testPayload, len * sizeof(uint16_t)));

    // Clean up
    free(msg);
}

TEST(mag16, create_mag16_TOO_Long) {
    uint16_t pay[MSG_16_PAYLOAD_LEN + 1] = {0};
    msg16_t *p_actual;
    p_actual = create_msg16(1, 1, 1, MSG_16_PAYLOAD_LEN + 1, pay);
    EXPECT_EQ(p_actual->len, 0); // Ensure memory allocation was successful    free(p_actual);
}


TEST(mag16, destructor_GOLDEN) {
    msg16_t *p;
    p = (msg16_t *) malloc(sizeof(msg16_t));
    int ret = destruct_msg16(&p);
    ASSERT_EQ(p, nullptr);
    ASSERT_EQ(ret, 1);

}


