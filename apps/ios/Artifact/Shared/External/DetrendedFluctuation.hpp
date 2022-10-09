//
//  DetrendedFluctuation.hpp
//

// Todo move shared service in monorepo root (artifact/services/detrended-fluctuation)

#ifndef DetrendedFluctuation_hpp
#define DetrendedFluctuation_hpp

#include <stdio.h>
#include <string>

class DetrendedFluctuation {
public:
    std::string sayHello();
    float computeDF(int rr);
};

#endif /* DetrendedFluctuation_hpp */
