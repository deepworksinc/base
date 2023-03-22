//
//  DFA.hpp
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
    float push(vector<int>& rrs);
    float _compute();
    void _initialize();
    vector<float> _scales(double a, double b, int k);
};

#endif /* DetrendedFluctuation_hpp */
