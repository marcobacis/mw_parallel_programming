//
//  GameStateView.m
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 05/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import <Accelerate/Accelerate.h>
#import "GameStateView.h"
#import "GameObject.h"


@implementation GameStateView {
  NSMutableDictionary <NSNumber *, GameObject *> *objects;
  double worldViewProjMtx[4*4];
}


- (instancetype)initWithCoder:(NSCoder *)decoder
{
  self = [super initWithCoder:decoder];
  objects = [NSMutableDictionary dictionary];
  memset(worldViewProjMtx, 0, sizeof(double)*4*4);
  return self;
}


- (void)updateObjects:(NSDictionary <NSNumber *, GameObject *> *)obj
{
  [objects addEntriesFromDictionary:obj];
  [self setNeedsDisplay:YES];
}


- (void)clearObjects
{
  [objects removeAllObjects];
  [self setNeedsDisplay:YES];
}


- (void)setWorldViewProjectionMatrix:(double *)wvpm
{
  memcpy(worldViewProjMtx, wvpm, sizeof(double)*4*4);
  [self setNeedsDisplay:YES];
}


- (void)setFieldRect:(NSRect)fieldRect
{
  _fieldRect = fieldRect;
  [self setNeedsDisplay:YES];
}


- (void)perspTransformPoint:(const double *)in_xyzh intoPoint:(double *)out_xy
{
  double out_xyzh[4];
  
  vDSP_mmulD(worldViewProjMtx, 1, in_xyzh, 1, out_xyzh, 1, 4, 1, 4);
  
  out_xy[0] = (out_xyzh[0] / out_xyzh[3] + 1.0) / 2.0 * self.bounds.size.width;
  out_xy[1] = (out_xyzh[1] / out_xyzh[3] + 1.0) / 2.0 * self.bounds.size.height;
}


- (void)drawRect:(NSRect)dirtyRect
{
  [[NSColor whiteColor] setFill];
  NSRectFill(dirtyRect);
  
  [[NSColor blackColor] setStroke];
  double field_tl[4] = {NSMinX(_fieldRect), NSMinY(_fieldRect), 0.0, 1.0};
  double field_tr[4] = {NSMaxX(_fieldRect), NSMinY(_fieldRect), 0.0, 1.0};
  double field_br[4] = {NSMaxX(_fieldRect), NSMaxY(_fieldRect), 0.0, 1.0};
  double field_bl[4] = {NSMinX(_fieldRect), NSMaxY(_fieldRect), 0.0, 1.0};
  double field_scoords[4*2];
  [self perspTransformPoint:field_tl intoPoint:field_scoords+0];
  [self perspTransformPoint:field_tr intoPoint:field_scoords+2];
  [self perspTransformPoint:field_br intoPoint:field_scoords+4];
  [self perspTransformPoint:field_bl intoPoint:field_scoords+6];
  
  NSBezierPath *field = [NSBezierPath bezierPath];
  [field moveToPoint:NSMakePoint(field_scoords[0], field_scoords[1])];
  [field lineToPoint:NSMakePoint(field_scoords[2], field_scoords[3])];
  [field lineToPoint:NSMakePoint(field_scoords[4], field_scoords[5])];
  [field lineToPoint:NSMakePoint(field_scoords[6], field_scoords[7])];
  [field lineToPoint:NSMakePoint(field_scoords[0], field_scoords[1])];
  [field closePath];
  [field stroke];
  
  [objects enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull objid, GameObject * _Nonnull obj, BOOL * _Nonnull stop) {
    double in_xyzh[4] = {obj.x, obj.y, obj.z, 1.0};
    double xy[2];
    
    [self perspTransformPoint:in_xyzh intoPoint:xy];
    
    [obj.color setFill];
    CGFloat oval_l = xy[0] - (obj.size / 2.0);
    CGFloat oval_t = xy[1] - (obj.size / 2.0);
    CGFloat oval_wh = obj.size;
    NSBezierPath *bp = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(oval_l, oval_t, oval_wh, oval_wh)];
    [bp fill];
    
    NSString *objidstr = [NSString stringWithFormat:@"%"PRIu64, (int64_t)objid.integerValue];
    [objidstr drawAtPoint:NSMakePoint(xy[0], xy[1]) withAttributes:@{}];
  }];
}


@end
