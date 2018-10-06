//
//  GameStateView.h
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 05/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@class GameObject;
@class Matrix;

@interface GameStateView : NSView

@property (nonatomic) Matrix *worldViewProjectionMatrix;
@property (nonatomic) NSRect fieldRect;
@property (nonatomic) double extraRadius;
- (void)updateObjects:(NSDictionary <NSNumber *, GameObject *> *)obj;
- (void)clearObjects;

@end

NS_ASSUME_NONNULL_END
