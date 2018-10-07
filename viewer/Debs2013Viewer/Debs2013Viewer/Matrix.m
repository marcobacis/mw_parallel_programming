//
//  Matrix.m
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 06/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import "Matrix.h"
#import <Accelerate/Accelerate.h>


@implementation Matrix


@synthesize elements = _elements;


- (instancetype)initWithMatrix:(Matrix *)m
{
  self = [super init];
  _rows = m.rows;
  _columns = m.columns;
  double *elems = calloc(_rows*_columns, sizeof(double));
  memcpy(elems, m.elements, _rows*_columns*sizeof(double));
  _elements = elems;
  return self;
}


- (instancetype)initWithRows:(int)rows columns:(int)cols elements:(const double *)m
{
  return [self initWithRows:rows columns:cols elements:m noCopy:NO];
}


- (instancetype)initWithRows:(int)rows columns:(int)cols elements:(const double *)m noCopy:(BOOL)own
{
  self = [super init];
  _rows = rows;
  _columns = cols;
  if (!own) {
    double *elems = calloc(_rows*_columns, sizeof(double));
    memcpy(elems, m, _rows*_columns*sizeof(double));
    _elements = elems;
  } else {
    _elements = m;
  }
  return self;
}


- (void)dealloc
{
  free((void *)_elements);
}


- (NSString *)description
{
  NSMutableString *res = [NSMutableString string];
  const double *p = self.elements;
  for (int i=0; i<self.rows; i++) {
    [res appendString:@"["];
    for (int j=0; j<self.columns; j++) {
      if (j != 0)
        [res appendString:@" "];
      [res appendFormat:@"%lf", *p];
      p++;
    }
    [res appendString:@"]\n"];
  }
  return res;
}


- (double *)row:(int)i
{
  return _elements + (i * self.columns);
}


- (Matrix *)multiply:(Matrix *)b
{
  if (self.columns != b.rows)
    [NSException raise:NSInvalidArgumentException format:@"Non-conformable matrices"];
  
  double *elems = calloc(self.rows * b.columns, sizeof(double));
  vDSP_mmulD(self.elements, 1, b.elements, 1, elems, 1, self.rows, b.columns, self.columns);
  return [[[self class] alloc] initWithRows:self.rows columns:b.columns elements:elems noCopy:YES];
}


+ (Matrix *)translationOfX:(double)x y:(double)y z:(double)z
{
  double elems[4*4] = {
    1.0, 0.0, 0.0,   x,
    0.0, 1.0, 0.0,   y,
    0.0, 0.0, 1.0,   z,
    0.0, 0.0, 0.0, 1.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


+ (Matrix *)scalingByX:(double)x y:(double)y z:(double)z
{
  double elems[4*4] = {
      x, 0.0, 0.0, 0.0,
    0.0,   y, 0.0, 0.0,
    0.0, 0.0,   z, 0.0,
    0.0, 0.0, 0.0, 1.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


+ (Matrix *)rotationAroundXAxisOf:(double)rad
{
  double elems[4*4] = {
          1.0,       0.0,       0.0, 0.0,
          0.0,  cos(rad), -sin(rad), 0.0,
          0.0,  sin(rad),  cos(rad), 0.0,
          0.0,       0.0,       0.0, 1.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


+ (Matrix *)rotationAroundYAxisOf:(double)rad
{
  double elems[4*4] = {
     cos(rad),       0.0,  sin(rad), 0.0,
          0.0,       1.0,       0.0, 0.0,
    -sin(rad),       0.0,  cos(rad), 0.0,
          0.0,       0.0,       0.0, 1.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


+ (Matrix *)rotationAroundZAxisOf:(double)rad
{
  double elems[4*4] = {
     cos(rad), -sin(rad),       0.0, 0.0,
     sin(rad),  cos(rad),       0.0, 0.0,
          0.0,       0.0,       1.0, 0.0,
          0.0,       0.0,       0.0, 1.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


+ (Matrix *)perspectiveProjectionWithFovY:(double)fovy nearPlane:(double)n farPlane:(double)f aspect:(double)a
{
  double elems[4*4] = {
     1.0/(a*tan(fovy/2.0)),                  0.0,               0.0,          0.0,
                       0.0,  1.0/(tan(fovy/2.0)),               0.0,          0.0,
                       0.0,                  0.0,       (f+n)/(n-f),  2*f*n/(n-f),
                       0.0,                  0.0,              -1.0,          0.0,
  };
  return [[[self class] alloc] initWithRows:4 columns:4 elements:elems];
}


@end
