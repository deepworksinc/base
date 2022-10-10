//
//  DetrendedFluctuationWrapper.m
//

#include <Foundation/Foundation.h>
#include "DetrendedFluctuationWrapper.h"
#include "DetrendedFluctuation.hpp"
#include <vector>

using std::vector;

@implementation DetrendedFluctuationWrapper
- (NSNumber *) computeDF: (NSArray *) rr {
    DetrendedFluctuation detrendedFluctuation;
    vector<int> rrArray;
    for (int j = 0; j < [rr count]; ++j) {
        int val = [rr[j] intValue];
        rrArray.push_back(val);
    }
    float df = detrendedFluctuation.computeDF(rrArray);
    return @(df);
}
@end
