//
//  DFA1Wrapper.m
//

#include <Foundation/Foundation.h>
#include "DFA1Wrapper.h"
#include "DFA1.hpp"
#include <vector>

using std::vector;

@implementation DFA1Wrapper
- (NSNumber *) push: (NSArray *) rrs {
    DFA1 dfa1;
    vector<int> rrs_vector;
    for (int j = 0; j < [rrs count]; ++j) {
        int val = [rrs[j] intValue];
        rrs_vector.push_back(val);
    }
    float dfa1 = DFA1.push(rrs_vector);
    return @(dfa1);
}
@end
