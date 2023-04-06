//
//  HRVWrapper.m
//

#include <Foundation/Foundation.h>
#include "HRVWrapper.h"
#include "HRV.hpp"
#include <vector>

using std::vector;

@implementation HRVWrapper
- (NSDictionary *) computeFeatures: (NSArray *) wrapperInput {
    HRV hrv;
    vector<uint8_t> input;
    for (int j = 0; j < [wrapperInput count]; ++j) {
        int val = [wrapperInput[j] intValue];
        input.push_back(val);
    }
    Features features = hrv.computeFeatures(input);
    NSNumber *bpm = [NSNumber numberWithInt:features.bpm];
    NSNumber *dfa1 = [NSNumber numberWithFloat:features.dfa1];
    NSNumber *rmssd = [NSNumber numberWithFloat:features.rmssd];
    NSDictionary *result = @{@"bpm": bpm, @"dfa1": dfa1, @"rmssd": rmssd};
    return result;
}
@end
