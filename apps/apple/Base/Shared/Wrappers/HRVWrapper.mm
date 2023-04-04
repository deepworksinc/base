//
//  HRVWrapper.m
//

#include <Foundation/Foundation.h>
#include "HRVWrapper.h"
#include "HRV.hpp"
#include <vector>

using std::vector;

@implementation HRVWrapper
- (NSDictionary *) push: (NSArray *) bytes {
    HRV hrv;
    vector<uint8_t> bytes_vector;
    for (int j = 0; j < [bytes count]; ++j) {
        int val = [bytes[j] intValue];
        bytes_vector.push_back(val);
    }
    Features features = hrv.push(bytes_vector);
    NSNumber *bpm = [NSNumber numberWithInt:features.bpm];
    NSNumber *dfa1 = [NSNumber numberWithFloat:features.dfa1];
    NSNumber *rmssd = [NSNumber numberWithFloat:features.rmssd];
    NSDictionary *result = @{@"bpm": bpm, @"dfa1": dfa1, @"rmssd": rmssd};
    return result;
}
@end
