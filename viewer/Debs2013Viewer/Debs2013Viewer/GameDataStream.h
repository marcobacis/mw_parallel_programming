//
//  GameDataStream.h
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 06/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class GameObject;

@interface GameDataStream : NSObject

- (instancetype)initWithDataFile:(NSURL *)path;
@property (readonly) uint64_t startTime;
@property (readonly) uint64_t endTime;
@property (readonly) uint64_t currentTime;
- (void)skipToTime:(uint64_t)t;
- (NSDictionary<NSNumber *, GameObject *> *)advanceByTimeInterval:(uint64_t)dt;

@end

NS_ASSUME_NONNULL_END
