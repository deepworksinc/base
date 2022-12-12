//
//  DetrendedFluctuationWrapper.m
//

#include <Foundation/Foundation.h>
#include "DetrendedFluctuationWrapper.h"
#include "DetrendedFluctuation.hpp"
#include <vector>

using std::vector;

@implementation DetrendedFluctuationWrapper
- (NSNumber *) push: (NSArray *) rrs {
    DetrendedFluctuation detrended_fluctuation;
    vector<int> rrs_vector;
    for (int j = 0; j < [rrs count]; ++j) {
        int val = [rrs[j] intValue];
        rrs_vector.push_back(val);
    }
    float df = detrended_fluctuation.push(rrs_vector);
    return @(df);
}
@end
