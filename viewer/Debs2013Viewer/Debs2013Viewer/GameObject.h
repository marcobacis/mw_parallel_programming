//
//  GameObject.h
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 05/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import <Cocoa/Cocoa.h>


NS_ASSUME_NONNULL_BEGIN

@interface GameObject : NSObject

- (instancetype)initWithColor:(NSColor *)color size:(CGFloat)size label:(NSString *)label;
- (instancetype)initWithGameObject:(GameObject *)obj;

@property (nonatomic) NSColor *color;
@property (nonatomic) CGFloat size;
@property (nonatomic) NSString *label;
@property (nonatomic) CGFloat x;
@property (nonatomic) CGFloat y;
@property (nonatomic) CGFloat z;
@property (nonatomic) BOOL drawExtraRadius;

@end

NS_ASSUME_NONNULL_END
