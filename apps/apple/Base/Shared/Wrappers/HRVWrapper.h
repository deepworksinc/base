//
//  HRVWrapper.h
//

#ifndef HRVWrapper_h
#define HRVWrapper_h

#import <Foundation/Foundation.h>

@interface HRVWrapper : NSObject
- (NSDictionary *) computeFeatures: (NSArray *) bytes;
@end

#endif /* HRVWrapper_h */
