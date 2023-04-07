#include "HRV.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>
#include <Eigen/Dense>

using std::accumulate;
using std::cout;
using std::deque;
using std::transform;
using std::vector;
using Eigen::MatrixXf;

// HRV service state data
State state;

// HRV statistical constants
const int maxBufferSize = 120;
const float artifactCorrectionThreshold = 0.05;
const int lowerScaleLimit = 4;
const int upperScaleLimit = 16;
const int scaleDensity = 30;
const int m = 1; // Polynomial order m
const float start = log(lowerScaleLimit) / log(10);
const float stop = log(upperScaleLimit) / log(10);
vector<float> scales;
vector<int> F;
int count = 0;

// Push input data and return features
Features HRV::computeFeatures(vector<uint8_t>& input) {

    if (!state.initialized) {

        cout << "Initializing HRV feature computation constants..." << std::endl;

        initialize();
    }

    // Decode bytes
    DecodedInput decodedInput = decodeInput(input);

    // Update state bpmHistory
    state.bpmHistory.push_back(decodedInput.bpm);

    // Loop over RR values and update state rrHistory and rrBuffer
    for (int i = 0; i < decodedInput.rrs.size(); ++i) {
        
        // Artifact Correction Threshold 0.05
        int newRR = decodedInput.rrs[i];
        if (state.rrBuffer.size()) {
            int lastRR = state.rrBuffer[state.rrBuffer.size() - 1];
            float lowerCorrectionThreshold = lastRR * (1 - artifactCorrectionThreshold);
            float upper_correction_threshold = lastRR * (1 + artifactCorrectionThreshold);
            if (lowerCorrectionThreshold < newRR < upper_correction_threshold) {
                state.rrHistory.push_back(newRR);
                state.rrBuffer.push_back(newRR);
            };
        } else {
            state.rrHistory.push_back(newRR);
            state.rrBuffer.push_back(newRR);
        }

        // Remove oldest RR value from buffer
        if (state.rrBuffer.size() > maxBufferSize) {
            state.rrBuffer.pop_front();
        }
    }
    
    float dfa1 = computeDFA1();
    float rmssd = computeRMSSD();

    // Update state dfa1History and rmssdHistory
    state.dfa1History.push_back(dfa1);
    state.rmssdHistory.push_back(rmssd);
    
    return {
        decodedInput.bpm,
        dfa1,
        rmssd
    };
}

float HRV::computeDFA1() {
    return float(0);
}

float HRV::computeRMSSD() {
    return float(0);
}

// // Compute DFA1 feature
// float HRV::computeDFA1(vector<int>* rrBuffer) {
    
//     for (float s: scales) {
//         vector<float> rms;
        
//         // Step 1: Determine the "profile" (integrated signal with subtracted offset)
//         float L = rrBuffer.size();
//         float mean = accumulate(rrBuffer.begin(), rrBuffer.end(), 0) / L;
//         vector<float> rrsSubMean(L);
//         for (int i = 0; i < L; ++i) {
//             rrsSubMean[i] = rrBuffer[i] - mean;
//         }
//         vector<int> yN(rrsSubMean.size());
//         partial_sum(rrsSubMean.begin(), rrsSubMean.end(), yN.begin());
            
        // // Step 2: Divide the profile into N non-overlapping segments of equal length s
        // int N = floor(L / s);
        // vector<vector<float>> yNSegments(N);
        // for (int i = 0; i < N; ++i) {
        //     yNSegments[i] = vector<float>(s);
        //     for (int j = 0; j < s; ++j) {
        //         yNSegments[i][j] = yN[i * s + j];
        //     }
        // }
        
        // // Step 3: Calculate the local trend for each 2Ns segments by a least squares fit of the series
        // for (int i = 0; i < N; ++i) {
        //
        //     ... TODO
        //
        //     rms.push_back(rms_i);
        // }
        
        // // Step 4: Calculate the root mean square (RMS) of the detrended profile
        // float rmsMean = accumulate(rms.begin(), rms.end(), 0) / N;
        // F[count] = rmsMean;
        // count++;
    // }

    // return 1.0;
    
    // // Step 5: Calculate the scaling exponent alpha 1
    // vector<float> logF;
    // for (int i = 0; i < F.size(); ++i) {
    //     logF.push_back(log(F[i]));
    // }
    // vector<float> logS;
    // for (int i = 0; i < scales.size(); ++i) {
    //     logS.push_back(log(scales[i]));
    // }
    // float logFMean = accumulate(logF.begin(), logF.end(), 0) / logF.size();
    // float logSMean = accumulate(logS.begin(), logS.end(), 0) / logS.size();
    // float numerator = 0;
    // float denominator = 0;
    // for (int i = 0; i < logF.size(); ++i) {
    //     numerator += (logF[i] - logFMean) * (logS[i] - logSMean);
    //     denominator += pow(logS[i] - logSMean, 2);
    // }
    // return numerator / denominator;
// }

// Initialize the HRV object
void HRV::initialize() {
    scales = createScales(log10(pow(10, start)), log10(pow(10, stop)), scaleDensity);
    F.resize(scales.size());
    state.initialized = true;
}


// Decode the raw byte input data
DecodedInput HRV::decodeInput(vector<uint8_t>& input) {
    int bpm = 0;
    int firstBit = input[0] & 1;
    if (firstBit == 0) {
        // BPM value Format is in the 2nd byte
        bpm = int(input[1]);
    } else {
        // Heart Rate Value Format is in the 2nd and 3rd bytes
        bpm = (int(input[1]) << 8) + int(input[2]);
    }
    vector<int> rrs;
    int fifthBit = (input[0] & 16);
    if (fifthBit > 0) {
        if (input[0] % 2 == 0 && input[0] >= 16 && input[0] <= 22) {
            //rr-value in [2] und [3]
            rrs.push_back(int(input[2]) + (int(input[3]) << 8));
        }
        if (input[0] % 2 != 0 && input[0] >= 17 && input[0] <= 23) {
            //rr-value in [3] und [4]
            rrs.push_back(int(input[3]) + (int(input[4]) << 8));
        }
        if (input[0] % 2 == 0 && input[0] >= 24 && input[0] <= 30) {
            //rr-value in [4] und [5]
            rrs.push_back(int(input[4]) + (int(input[5]) << 8));
        }
        if (input[0] % 2 != 0 && input[0] >= 25 && input[0] <= 31) {
            //rr-value in [5] und [6]
            rrs.push_back(int(input[5]) + (int(input[6]) << 8));
        }
    }
    return DecodedInput{bpm, rrs};
}

// Generate a vector of scales
vector<float> HRV::createScales(double a, double b, int k) {
    const auto exp_scale = (b - a) / (k - 1);
    vector<double> logspace;
    logspace.reserve(k);
    for (int i = 0; i < k; i++) {
        logspace.push_back(i * exp_scale);
    }
    vector<float> scales;
    for_each(logspace.begin(), logspace.end(), [&scales](double &x) {
        scales.push_back(floor(pow(10, x)));
    });
    return scales;
}
