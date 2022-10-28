//
//  DetrendedFluctuation.hpp
//

// Todo move shared service in monorepo root (base/services/detrended-fluctuation)

#ifndef DetrendedFluctuation_hpp
#define DetrendedFluctuation_hpp

#include <stdio.h>
#include <string>
#include <vector>

using std::vector;

class DetrendedFluctuation {
public:
    float compute(vector<int>& rrs);
};

#endif /* DetrendedFluctuation_hpp */