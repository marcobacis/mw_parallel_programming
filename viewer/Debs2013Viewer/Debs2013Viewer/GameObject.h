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

@property NSColor *color;
@property CGFloat size;
@property CGFloat x;
@property CGFloat y;
@property CGFloat z;

@end

NS_ASSUME_NONNULL_END
