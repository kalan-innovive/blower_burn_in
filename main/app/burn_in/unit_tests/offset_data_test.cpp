//
// Created by Erik Klan on 3/17/24.
//

#ifdef __cplusplus
extern "C" {
#endif
#include "register.h"
#include "reg_types.h"
#include "dev_types.h"
#include "offset_data.h"
#ifdef __cplusplus
}
#endif

#include "gtest/gtest.h"

#include "offset_data.h"
#include "gtest/gtest.h"

// Assuming LEN_CHIPDATA_ARRAY is defined as the length of chipArray.
// If not, define it for your tests or use sizeof(chipArray)/sizeof(chipArray[0]).
extern ChipData chipArray[10];

class OffsetDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize chipArray or other global setups
        init_chipArrray();

    }

    void TearDown() override {
        // Clean up if necessary
    }
};

TEST_F(OffsetDataTest, InitChipArray) {
    // Validate that chipArray is initialized correctly
    for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
        EXPECT_EQ(chipArray[i].chipID, 0);
        // Add more checks as necessary
    }
}

TEST_F(OffsetDataTest, AddChipData) {
    ChipData cd = {12345, -50, -60};
    int index = addChipData(&cd);
    EXPECT_GE(index, 0);
    EXPECT_EQ(chipArray[index - 1].chipID, 12345);
    // Add more checks as necessary
}

TEST_F(OffsetDataTest, GetChipData) {
    // Assuming addChipData works correctly, or you could manually populate chipArray
    ChipData cd = {67890, 10, 20};
    addChipData(&cd);
    ChipData *result = getChipData(67890);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->chipID, 67890);
    // Add more checks as necessary
}


TEST_F(OffsetDataTest, OffsetMin) {
    // Assuming chip data has been added and offsets populated for test
    ChipData cd = {1234576, 10, 20};
    addChipData(&cd);
    // Simulate adding offsets directly to cd.offset_array for testing
    // You'll need to do this setup based on how addOffset and the circular array work
    addOffset(cd.chipID, 5);
    addOffset(cd.chipID, -5);

    ChipData *data = getChipData(1234576);
    ASSERT_NE(data, nullptr);
    int minOffset = offset_min(data);
    EXPECT_EQ(minOffset, -5);
    data->qc_offset = -10;
    minOffset = offset_min(data);
    EXPECT_EQ(minOffset, -10);
    data->vas_offset = -20;
    minOffset = offset_min(data);
    EXPECT_EQ(minOffset, -20);

}

TEST_F(OffsetDataTest, OffsetMax) {
    ChipData cd = {654321, -10, -20};
    addChipData(&cd);
    addOffset(cd.chipID, -5);
    addOffset(cd.chipID, 5);

    ChipData *data = getChipData(654321);
    ASSERT_NE(data, nullptr);
    int maxOffset = offset_max(data);
    EXPECT_EQ(maxOffset, 5);
    data->qc_offset = 10;
    maxOffset = offset_max(data);
    EXPECT_EQ(maxOffset, 10);
    data->vas_offset = 20;
    maxOffset = offset_max(data);
    EXPECT_EQ(maxOffset, 20);

    // Add more checks as necessary
}

TEST_F(OffsetDataTest, OffsetRange) {
    ChipData cd = {111222, -1, 10};
    addChipData(&cd);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 15);

    ChipData *data = getChipData(111222);
    ASSERT_NE(data, nullptr);
    int range = offset_range(data);
    EXPECT_EQ(range, 16); // Assuming max - min calculation is correct
    // Add more checks as necessary
}

// Assuming get_min_last_n, get_max_last_n, and other related functions work correctly.
// You would write similar tests for offset_min_last_n, offset_max_last_n, and offset_range_last_n.

TEST_F(OffsetDataTest, AddOffset) {
    unsigned int chipID = 987654;
    ChipData cd = {chipID, 0, 0};
    addChipData(&cd);

    int result = addOffset(chipID, 25);
    EXPECT_EQ(result, 1); // Assuming the result is the new size of the offset array

    ChipData *data = getChipData(chipID);
    ASSERT_NE(data, nullptr);
    // Validate that the offset was added correctly
    EXPECT_EQ(getCurrOffset(data), 25);
    // Add more checks as necessary
}


TEST_F(OffsetDataTest, OffsetMin_N) {
    // Assuming chip data has been added and offsets populated for test
    ChipData cd = {123456, 10, 20};
    addChipData(&cd);
    // Simulate adding offsets directly to cd.offset_array for testing
    // You'll need to do this setup based on how addOffset and the circular array work
    addOffset(cd.chipID, 5);
    addOffset(cd.chipID, -5);

    ChipData *data = getChipData(123456);
    ASSERT_NE(data, nullptr);
    int minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -5);
    addOffset(cd.chipID, -20);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -20);
    addOffset(cd.chipID, 0);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -20);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 0);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -20);
    addOffset(cd.chipID, 0);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, 0);
    data->qc_offset = -15;
    addOffset(cd.chipID, -11);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -15);
    data->vas_offset = -16;
    addOffset(cd.chipID, -10);
    minOffset = offset_min_last_n(data, 4);
    EXPECT_EQ(minOffset, -16);


    // Add more checks for boundary conditions and other scenarios
}

TEST_F(OffsetDataTest, OffsetMax_N) {
    ChipData cd = {654321, -10, -20};
    addChipData(&cd);
    addOffset(cd.chipID, 25);
    addOffset(cd.chipID, -5);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 5);

    ChipData *data = getChipData(654321);
    ASSERT_NE(data, nullptr);
    int maxOffset = offset_max_last_n(data, 4);
    EXPECT_EQ(maxOffset, 5);

}

TEST_F(OffsetDataTest, OffsetRange_N) {
    ChipData cd = {111222, 6, 10};
    addChipData(&cd);

    addOffset(cd.chipID, 25);
    addOffset(cd.chipID, -5);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 0);
    addOffset(cd.chipID, 5);
//    addOffset(cd.chipID, 15);

    ChipData *data = getChipData(111222);
    ASSERT_NE(data, nullptr);
    int range = offset_range_last_n(data, 4);
    EXPECT_EQ(range, 10);
    data->qc_offset = 0xFFFE;
    range = offset_range_last_n(data, 4);
    EXPECT_EQ(range, 6);
    data->vas_offset = 0xFFFE;
    range = offset_range_last_n(data, 4);
    EXPECT_EQ(range, 5);


}


TEST_F(OffsetDataTest, GetCurrOffset) {
    unsigned int chipID = 192837;
    ChipData cd = {chipID, 0, 0};
    addChipData(&cd);
    addOffset(chipID, -10);
    addOffset(chipID, 20);

    ChipData *data = getChipData(chipID);
    ASSERT_NE(data, nullptr);
    int currOffset = getCurrOffset(data);
    EXPECT_EQ(currOffset, 20); // Assuming the latest added offset is returned

}

