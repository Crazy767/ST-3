// Copyright 2024 Skotin Alexander
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Throw;

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor* door;
    MockTimerClient mockClient;
    Timer timer;

    void SetUp() override {
        door = new TimedDoor(1);
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, DoorIsInitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensTheDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesTheDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorIsOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowWhenDoorIsClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, TimeoutCalledOnOpenDoor) {
    door->unlock();
    EXPECT_CALL(mockClient, Timeout())
        .Times(1)
        .WillOnce(Throw(std::runtime_error("The door is still open!")));
    ASSERT_THROW(timer.tregister(1, &mockClient), std::runtime_error);
}

TEST_F(TimedDoorTest, NoTimeoutCalledOnClosedDoor) {
    door->lock();
    EXPECT_CALL(mockClient, Timeout())
        .Times(0);
    timer.tregister(1, &mockClient);
}

TEST_F(TimedDoorTest, DoorStateAfterTimeoutException) {
    door->unlock();
    try {
        door->throwState();
    }
    catch (const std::runtime_error&) {}
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ReLockDoorBeforeTimeout) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ExceptionIfDoorReopenedAfterTimeoutSet) {
    door->unlock();
    door->lock();
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}
