/*
 * Copyright 2022 University of Michigan EECS183
 *
 * AI.cpp
 * Project UID 28eb18c2c1ce490aada441e65559efdd
 *
 * Congyue Yang, Sida Tian, Yuxuan Liu, Li Yuan
 * cyueyang, startian, lyxuan, leeyuan
 *
 * Final Project - Elevators
 */

#include "AI.h"
#include <cassert>


bool allElevatorsInServcing(const BuildingState building);
bool noPeople(const BuildingState building);
int numPeopleWaiting(const BuildingState building);
int numFloorHasPeople(const BuildingState building);
int floorWithMostPeople(const BuildingState building);
int numPeopleInFloorWithMostPeople(const BuildingState building);
double angerAVG(const BuildingState building);
double angerSD(const BuildingState building);
double floorAVG(const BuildingState building);
double floorSD(const BuildingState building);
double tscore(const BuildingState building, double sd, double avg, double x, int n);
int floorWithMostAnger(const BuildingState building);
double sumAngerInFloorWithMostAnger(const BuildingState building);
int chooseElevator(const BuildingState building, int targetFloor);

bool allElevatorsInServcing(const BuildingState building) {
    if (building.elevators[0].isServicing &&
        building.elevators[1].isServicing &&
        building.elevators[2].isServicing) {
        return true;
    }
    else {
        return false;
    }
}

bool noPeople(const BuildingState building) {
    for (int i = 0; i < NUM_FLOORS; i++) {
        if (building.floors[i].numPeople > 0) {
            return false;
        }
    }
    return true;
}

int numPeopleWaiting(const BuildingState building) {
    int sum = 0;
    for (int i = 0; i < NUM_FLOORS; i++) {
        sum = sum + building.floors[i].numPeople;
    }
    return sum;
}

int numFloorHasPeople(const BuildingState building) {
    int num = 0;
    for (int i = 0; i < NUM_FLOORS; i++) {
        if (building.floors[i].numPeople > 0) {
            num = num + 1;
        }
    }
    return num;
}

int floorWithMostPeople(const BuildingState building) {
    int numPeopleMax = 0;
    int numPeopleOnFloor = 0;
    int floorHasMostPeople;
    for (int i = 0; i < NUM_FLOORS; i++) {
        numPeopleOnFloor = building.floors[i].numPeople;
        if (numPeopleOnFloor >= numPeopleMax) {
            numPeopleMax = numPeopleOnFloor;
            floorHasMostPeople = i;
        }
    }
    return floorHasMostPeople;
}

int numPeopleInFloorWithMostPeople(const BuildingState building) {
    return building.floors[floorWithMostPeople(building)].numPeople;
}

double angerAVG(const BuildingState building) {
    double sum = 0;
    double average = 0;
    for (int i = 0; i < NUM_FLOORS; i++) {
        for (int j = 0; j < building.floors[i].numPeople; j++) {
            sum = sum + building.floors[i].people[j].angerLevel;
        }
    }
    average = sum / numPeopleWaiting(building);
    return average;
}

double angerSD(const BuildingState building) {
    double SD = 0;
    double variance = 0;
    double average = angerAVG(building);
    for (int i = 0; i < NUM_FLOORS; i++) {
        for (int j = 0; j < building.floors[i].numPeople; j++) {
            variance = variance + ((building.floors[i].people[j].angerLevel - average) *
                (building.floors[i].people[j].angerLevel - average));
        }
    }
    SD = sqrt(variance / numPeopleWaiting(building));
    return SD;
}

double floorAVG(const BuildingState building) {
    double sum = 0;
    double average = 0;
    for (int i = 0; i < NUM_FLOORS; i++) {
        sum = sum + building.floors[i].numPeople;
    }
    average = sum / numFloorHasPeople(building);
    return average;
}

double floorSD(const BuildingState building) {
    double SD = 0;
    double variance = 0;
    double average = floorAVG(building);
    for (int i = 0; i < NUM_FLOORS; i++) {
        variance = variance + ((building.floors[i].numPeople - average) *
            (building.floors[i].numPeople - average));
    }
    SD = sqrt(variance / numFloorHasPeople(building));
    return SD;
}

double tscore(const BuildingState building, double sd, double avg, double x, int n) {
    double result;
    result = (x - avg) / (sd / sqrt(n));
    return result;
}

int floorWithMostAnger(const BuildingState building) {
    int angerLevelMax = 0;
    int floorHasMostAnger;
    for (int i = 0; i < NUM_FLOORS; i++) {
        int angerLevelOnFloor = 0;
        for (int j = 0; j < building.floors[i].numPeople; j++) {
            angerLevelOnFloor = angerLevelOnFloor + building.floors[i].people[j].angerLevel;
        }
        if (angerLevelOnFloor >= angerLevelMax) {
            angerLevelMax = angerLevelOnFloor;
            floorHasMostAnger = i;
        }
    }
    return floorHasMostAnger;
}

double sumAngerInFloorWithMostAnger(const BuildingState building) {
    double sum = 0;
    for (int i = 0; i < building.floors[floorWithMostAnger(building)].numPeople; i++) {
        sum = sum + building.floors[floorWithMostAnger(building)].people[i].angerLevel;
    }
    return sum;
}

int chooseElevator(const BuildingState building, int targetFloor) {
    int elevatorIndex = -1;
    int findFloor = NUM_FLOORS;
    for (int i = 0; i < NUM_ELEVATORS; i++) {
        if (!building.elevators[i].isServicing) {
            if (abs(building.elevators[i].currentFloor - targetFloor) < findFloor) {
                findFloor = abs(building.elevators[i].currentFloor - (targetFloor));
                elevatorIndex = i;
            }
        }
    }
    return elevatorIndex;
}

string getAIMoveString(const BuildingState& buildingState) {
    int floorToGo;
    if (floorWithMostPeople(buildingState) == floorWithMostAnger(buildingState)) {
        floorToGo = floorWithMostAnger(buildingState);
    }
    else if (tscore(buildingState, angerSD(buildingState), angerAVG(buildingState),
                    sumAngerInFloorWithMostAnger(buildingState), numPeopleWaiting(buildingState)) >
             tscore(buildingState, floorSD(buildingState), floorAVG(buildingState),
                    buildingState.floors[floorWithMostAnger(buildingState)].numPeople, NUM_FLOORS)) {
        floorToGo = floorWithMostAnger(buildingState);
    }
    else if (tscore(buildingState, angerSD(buildingState), angerAVG(buildingState),
                    sumAngerInFloorWithMostAnger(buildingState), numPeopleWaiting(buildingState)) <
             tscore(buildingState, floorSD(buildingState), floorAVG(buildingState),
                    buildingState.floors[floorWithMostAnger(buildingState)].numPeople, NUM_FLOORS)) {
        floorToGo = floorWithMostPeople(buildingState);
    }
    else {
        floorToGo = floorWithMostPeople(buildingState);
    }

    int elevatorToService = chooseElevator(buildingState, floorToGo);

    if (allElevatorsInServcing(buildingState) ||
        noPeople(buildingState)) {
        return "";
    }

    for (int i = 0; i < NUM_ELEVATORS; i++) {
        if (!buildingState.elevators[i].isServicing &&
            (buildingState.floors[buildingState.elevators[i].currentFloor].numPeople > 0)) {
            string pickupString = "e" + 
                to_string(i) + 
                "p";
            return pickupString;
        }
    }

    if (buildingState.floors[buildingState.elevators[elevatorToService].currentFloor].numPeople > 0 &&
        buildingState.elevators[elevatorToService].currentFloor == floorToGo) {
        string pickupMoveString = "e" +
            to_string(elevatorToService) +
            "p";
        return pickupMoveString;
    }
    
    if (buildingState.elevators[elevatorToService].currentFloor != floorToGo) {
        string serviceString = "e" +
            to_string(elevatorToService) +
            "f" +
            to_string(floorToGo);
        return serviceString;
    }

    return "";
}

string getAIPickupList(const Move& move, const BuildingState& buildingState,
    const Floor& floorToPickup) {

    string peopleToPick = "";
    string peopleUpList = "";
    string peopleDownList = "";
    int peopleUp = 0;
    int peopleDown = 0;
    int peopleUpAnger = 0;
    int peopleDownAnger = 0;

    for (int i = 0; i < floorToPickup.getNumPeople(); i++) {
        if ((floorToPickup.getPersonByIndex(i).getTargetFloor() > buildingState.elevators[move.getElevatorId()].currentFloor)) {
            peopleUpList += to_string(i);
            ++peopleUp;
            peopleUpAnger += buildingState.floors[buildingState.elevators[move.getElevatorId()].currentFloor].people[i].angerLevel;
        }
        else if ((floorToPickup.getPersonByIndex(i).getTargetFloor() < buildingState.elevators[move.getElevatorId()].currentFloor)) {
            peopleDownList += to_string(i);
            ++peopleDown;
            peopleDownAnger += buildingState.floors[buildingState.elevators[move.getElevatorId()].currentFloor].people[i].angerLevel;
        }
    }

    if (peopleUp > peopleDown) {
        if (peopleToPick.length() < ELEVATOR_CAPACITY) {
            peopleToPick = peopleUpList;
        }
    }
    else if (peopleUp < peopleDown) {
        if (peopleToPick.length() < ELEVATOR_CAPACITY) {
            peopleToPick = peopleDownList;
        }
    }
    else {
        if (peopleUpAnger >= peopleDownAnger) {
            peopleToPick = peopleUpList;
        }
        else if (peopleUpAnger < peopleDownAnger) {
            peopleToPick = peopleDownList;
        }
    }
    return peopleToPick;
}
