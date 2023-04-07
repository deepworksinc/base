//
//  HRV.hpp
//

#ifndef HRV_hpp
#define HRV_hpp

#include <stdio.h>
#include <string>
#include <vector>

using std::deque;
using std::vector;

// Service input data
struct DecodedInput
{
    int bpm;
    vector<int> rrs;
};

// Service state data
struct State
{
    bool initialized;
    vector<float> bpmHistory;
    vector<float> rrHistory;
    deque<float> rrBuffer;
    vector<float> dfa1History;
    vector<float> rmssdHistory;
};

// Service return data
struct Features
{
    int bpm;
    float dfa1;
    float rmssd;
};

// HRV class for raw data processing and feature computation
class HRV
{
public:
    Features computeFeatures(vector<uint8_t> &input);
    void initialize();
    float computeDFA1(vector<int>* rr_buffer);
    float computeRMSSD(vector<int>* rr_buffer);
    DecodedInput decodeInput(vector<uint8_t> &input);
    vector<float> createScales(double a, double b, int k);
};

#endif /* HRV_hpp */
