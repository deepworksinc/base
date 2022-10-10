//
//  DetrendedFluctuation.hpp
//

// Todo move shared service in monorepo root (artifact/services/detrended-fluctuation)

#ifndef DetrendedFluctuation_hpp
#define DetrendedFluctuation_hpp

#include <stdio.h>
#include <string>
#include <vector>

using std::vector;

class DetrendedFluctuation {
public:
    float computeDF(vector<int>& rr);
};

#endif /* DetrendedFluctuation_hpp */
