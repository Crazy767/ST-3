// Copyright 2024 Skotin Alexander
#include "TimedDoor.h"
#include <unistd.h>
#include <stdexcept>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("The door is still open!");
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        Timer timer;
        timer.tregister(iTimeout, adapter);
    }
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    ::sleep(timeout);
    client->Timeout();
}

void Timer::sleep(int seconds) {
    ::sleep(seconds);
}
