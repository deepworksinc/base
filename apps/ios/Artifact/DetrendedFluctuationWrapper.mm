//
//  DetrendedFluctuationWrapper.m
//

#import <Foundation/Foundation.h>
#import "DetrendedFluctuationWrapper.h"
#import "DetrendedFluctuation.hpp"

@implementation DetrendedFluctuationWrapper
- (NSString *) sayHello {
    DetrendedFluctuation detrendedFluctuation;
    std::string helloWorldMessage = detrendedFluctuation.sayHello();
    return [NSString
            stringWithCString:helloWorldMessage.c_str()
            encoding:NSUTF8StringEncoding];
}

- (NSNumber *) computeDF: (int) rr {
    DetrendedFluctuation detrendedFluctuation;
    float df = detrendedFluctuation.computeDF(rr);
    return @(df);
}
@end
