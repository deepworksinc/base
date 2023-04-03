//
//  HRV.hpp
//

#ifndef HRV_hpp
#define HRV_hpp

#include <stdio.h>
#include <string>
#include <vector>

using std::vector;

struct Data {
    int bpm;
    vector<int> rrs;
};

struct Features {
    int bpm;
    float dfa1;
    float rmssd;
};

class HRV {
public:
    Features push(vector<uint8_t>& bytes);
    float _compute();
    void _initialize();
    Data _decode(vector<uint8_t>& bytes);
    vector<float> _scales(double a, double b, int k);
};

#endif /* HRV_hpp */
