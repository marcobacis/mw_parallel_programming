//
//  GameObject.m
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 05/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import "GameObject.h"


@implementation GameObject


- (instancetype)initWithColor:(NSColor *)color size:(CGFloat)size label:(NSString *)label
{
  self = [super init];
  _color = color;
  _size = size;
  _label = label;
  return self;
}


- (instancetype)initWithGameObject:(GameObject *)obj
{
  self = [super init];
  _color = obj.color;
  _size = obj.size;
  _label = obj.label;
  _x = obj.x;
  _y = obj.y;
  _z = obj.z;
  _drawExtraRadius = obj.drawExtraRadius;
  return self;
}


@end
