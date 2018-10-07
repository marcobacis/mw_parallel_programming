//
//  Matrix.h
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 06/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface Matrix : NSObject

- (instancetype)initWithMatrix:(Matrix *)m;
- (instancetype)initWithRows:(int)rows columns:(int)cols elements:(const double *)m;

@property (readonly) int rows;
@property (readonly) int columns;
@property (readonly) const double *elements;
- (double *)row:(int)i;

- (Matrix *)multiply:(Matrix *)b;

+ (Matrix *)translationOfX:(double)x y:(double)y z:(double)z;
+ (Matrix *)scalingByX:(double)x y:(double)y z:(double)z;
+ (Matrix *)rotationAroundXAxisOf:(double)rad;
+ (Matrix *)rotationAroundYAxisOf:(double)rad;
+ (Matrix *)rotationAroundZAxisOf:(double)rad;
+ (Matrix *)perspectiveProjectionWithFovY:(double)fovy nearPlane:(double)n farPlane:(double)f aspect:(double)a;

@end

NS_ASSUME_NONNULL_END
