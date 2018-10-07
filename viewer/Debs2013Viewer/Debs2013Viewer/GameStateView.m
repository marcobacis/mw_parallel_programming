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
#import "Matrix.h"


@implementation GameStateView {
  NSMutableDictionary <NSNumber *, GameObject *> *objects;
  Matrix *_worldViewProjectionMatrix;
}


- (instancetype)initWithCoder:(NSCoder *)decoder
{
  self = [super initWithCoder:decoder];
  objects = [NSMutableDictionary dictionary];
  _worldViewProjectionMatrix = [Matrix scalingByX:1.0 y:1.0 z:0.0];
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


- (void)setProjectionMatrix:(Matrix *)projectionMatrix
{
  _projectionMatrix = projectionMatrix;
  _worldViewProjectionMatrix = [self.projectionMatrix multiply:self.viewMatrix];
  [self setNeedsDisplay:YES];
}


- (void)setViewMatrix:(Matrix *)viewMatrix
{
  _viewMatrix = viewMatrix;
  _worldViewProjectionMatrix = [self.projectionMatrix multiply:self.viewMatrix];
  [self setNeedsDisplay:YES];
}


- (void)setFieldRect:(NSRect)fieldRect
{
  _fieldRect = fieldRect;
  [self setNeedsDisplay:YES];
}


- (void)setExtraRadius:(double)extraRadius
{
  _extraRadius = extraRadius;
  [self setNeedsDisplay:YES];
}


- (void)perspTransformPoint:(const double *)in_xyzh intoPoint:(double *)out_xy
{
  double out_xyzh[4];
  
  vDSP_mmulD(_worldViewProjectionMatrix.elements, 1, in_xyzh, 1, out_xyzh, 1, 4, 1, 4);
  
  CGFloat w = self.bounds.size.width;
  CGFloat h = self.bounds.size.height;
  CGFloat side, zerox, zeroy;
  if (w > h) {
    side = h;
    zeroy = 0;
    zerox = (w - h) / 2.0;
  } else {
    side = w;
    zeroy = (h - w) / 2.0;
    zerox = 0;
  }
  out_xy[0] = (out_xyzh[0] / out_xyzh[3] + 1.0) / 2.0 * side + zerox;
  out_xy[1] = (out_xyzh[1] / out_xyzh[3] + 1.0) / 2.0 * side + zeroy;
}


- (void)drawRect:(NSRect)dirtyRect
{
  [[NSColor whiteColor] setFill];
  NSRectFill(dirtyRect);
  
  [[NSColor blackColor] setStroke];
  double field_tl[4] = {NSMinX(_fieldRect), NSMinY(_fieldRect), -100.0, 1.0};
  double field_tr[4] = {NSMaxX(_fieldRect), NSMinY(_fieldRect), -100.0, 1.0};
  double field_br[4] = {NSMaxX(_fieldRect), NSMaxY(_fieldRect), -100.0, 1.0};
  double field_bl[4] = {NSMinX(_fieldRect), NSMaxY(_fieldRect), -100.0, 1.0};
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
    [self drawObjectShadow:obj];
    if (obj.drawExtraRadius)
      [self drawRadius:self.extraRadius aroundObject:obj];
    [self drawObject:obj identifier:objid];
  }];
}


- (void)drawObjectShadow:(GameObject *)obj
{
  NSBezierPath *bp = [self pathForCircleOnXYPlaneWithCenterX:obj.x y:obj.y z:-100.0 radius:obj.size*100.0];
  [[NSColor colorWithCalibratedWhite:0.0 alpha:0.3] setFill];
  [bp fill];
}


- (void)drawObject:(GameObject *)obj identifier:(NSNumber *)objid
{
  double center_ws[4] = {obj.x, obj.y, obj.z, 1.0};
  double radvec_ws[4] = {
    obj.x+[self.viewMatrix row:0][0],
    obj.y+[self.viewMatrix row:0][1],
    obj.z+[self.viewMatrix row:0][2],
    1.0
  };
  double center_ss[2];
  double radvec_ss[2];
  
  [self perspTransformPoint:center_ws intoPoint:center_ss];
  [self perspTransformPoint:radvec_ws intoPoint:radvec_ss];
  
  double persp_corrected_rad = (radvec_ss[0] - center_ss[0]) * (obj.size / 2.0) * 200.0;
  
  [obj.color setFill];
  CGFloat oval_l = center_ss[0] - persp_corrected_rad;
  CGFloat oval_t = center_ss[1] - persp_corrected_rad;
  CGFloat oval_wh = persp_corrected_rad * 2.0;
  NSBezierPath *bp = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(oval_l, oval_t, oval_wh, oval_wh)];
  [bp fill];
  
  NSDictionary *attrib = @{NSForegroundColorAttributeName: obj.color};
  
  NSString *objidstr = [NSString stringWithFormat:@"%"PRIu64, (int64_t)objid.integerValue];
  [objidstr drawAtPoint:NSMakePoint(center_ss[0], center_ss[1]) withAttributes:attrib];
  
  if (obj.label) {
    [obj.label drawAtPoint:NSMakePoint(center_ss[0], center_ss[1]-11.0) withAttributes:attrib];
  }
}


- (NSBezierPath *)pathForCircleOnXYPlaneWithCenterX:(double)x y:(double)y z:(double)z radius:(double)rad
{
  double q_c = 0.55228;
  double xyzh_in[12][4] = {
    {    x - rad,           y, z, 1.0},
    {    x - rad, y + rad*q_c, z, 1.0},
    {x - rad*q_c,     y + rad, z, 1.0},
    {          x,     y + rad, z, 1.0},
    {x + rad*q_c,     y + rad, z, 1.0},
    {    x + rad, y + rad*q_c, z, 1.0},
    {    x + rad,           y, z, 1.0},
    {    x + rad, y - rad*q_c, z, 1.0},
    {x + rad*q_c,     y - rad, z, 1.0},
    {          x,     y - rad, z, 1.0},
    {x - rad*q_c,     y - rad, z, 1.0},
    {    x - rad, y - rad*q_c, z, 1.0},
  };
  
  double points[12][2];
  for (int i=0; i<12; i++) {
    [self perspTransformPoint:xyzh_in[i] intoPoint:points[i]];
  }
  
  NSBezierPath *bp = [NSBezierPath bezierPath];
  [bp moveToPoint:NSMakePoint(points[0][0], points[0][1])];
  for (int i=0; i<12; i+=3) {
    NSPoint dest = NSMakePoint(points[(i+3)%12][0], points[(i+3)%12][1]);
    NSPoint cp1 = NSMakePoint(points[(i+1)%12][0], points[(i+1)%12][1]);
    NSPoint cp2 = NSMakePoint(points[(i+2)%12][0], points[(i+2)%12][1]);
    [bp curveToPoint:dest controlPoint1:cp1 controlPoint2:cp2];
  }
  [bp closePath];
  return bp;
}


- (void)drawRadius:(double)rad aroundObject:(GameObject *)obj
{
  NSBezierPath *bp = [self pathForCircleOnXYPlaneWithCenterX:obj.x y:obj.y z:-100.0 radius:rad];
  [[obj.color colorWithAlphaComponent:.29] setStroke];
  [bp stroke];
}


@end
