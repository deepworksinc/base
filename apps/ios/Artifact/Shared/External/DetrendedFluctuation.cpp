//
//  DetrendedFluctuation.cpp
//

// Todo move shared service in monorepo root (artifact/services/detrended-fluctuation)

#include "DetrendedFluctuation.hpp"

std::string DetrendedFluctuation::sayHello() {
    return "Hello from CPP world!";
}

float DetrendedFluctuation::computeDF(int rr) {
    return float(rr);
}
