//
//  GameDataStream.m
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 06/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import "GameDataStream.h"
#import "GameObject.h"

#define IGNORED_PART_AT_END  256


@implementation GameDataStream {
  FILE *fp;
}


- (instancetype)initWithDataFile:(NSURL *)path
{
  self = [super init];
  const char *fspath = [path fileSystemRepresentation];
  fp = fopen(fspath, "r");
  if (!fp)
    return nil;
  
  _startTime = [self readCurrentRowAndAdvancing:NO][1].unsignedIntegerValue;
  fseeko(fp, 0, SEEK_END);
  off_t len = ftello(fp);
  [self lineAlignedSeek:len-IGNORED_PART_AT_END];
  _endTime = [self readCurrentRowAndAdvancing:NO][1].unsignedIntegerValue;
  fseeko(fp, 0, SEEK_SET);
  return self;
}


- (void)lineAlignedSeek:(off_t)where
{
  fseeko(fp, where, SEEK_SET);
  if (where == 0)
    return;
  
  size_t len;
  fgetln(fp, &len);
  return;
}


- (NSArray<NSNumber *> *)readCurrentRowAndAdvancing:(BOOL)adv
{
  if (feof(fp))
    return nil;
  
  off_t curpos = ftello(fp);
  size_t len;
  char *line = fgetln(fp, &len);
  int64_t sid, ts, x, y, z;
  sscanf(line, "%"PRId64",%"PRId64",%"PRId64",%"PRId64",%"PRId64, &sid, &ts, &x, &y, &z);
  if (feof(fp) || !adv) {
    fseeko(fp, curpos, SEEK_SET);
  }
  
  return @[@(sid), @(ts), @(x), @(y), @(z)];
}


- (uint64_t)currentTime
{
  NSArray<NSNumber *> *crow = [self readCurrentRowAndAdvancing:NO];
  return crow[1].integerValue;
}


- (void)skipToTime:(uint64_t)t
{
  uint64_t ct = [self currentTime];
  if (t == ct)
    return;
  
  off_t lowpos, hipos;
  if (t < ct) {
    lowpos = 0;
    hipos = ftello(fp);
  } else {
    lowpos = ftello(fp);
    fseeko(fp, 0, SEEK_END);
    hipos = ftello(fp) - IGNORED_PART_AT_END;
    fseeko(fp, lowpos, SEEK_SET);
  }
  
  while (lowpos != hipos) {
    off_t thispos = (lowpos + hipos) / 2;
    [self lineAlignedSeek:thispos];
    
    off_t real_thispos = ftello(fp);
    if (real_thispos == hipos) {
      fseeko(fp, lowpos, SEEK_SET);
      return;
    }
    
    ct = [self currentTime];
    if (t == ct) {
      return;
    } else if (t < ct) {
      hipos = real_thispos;
    } else {
      lowpos = real_thispos;
    }
  }
}


- (NSDictionary<NSNumber *, GameObject *> *)advanceByTimeInterval:(uint64_t)dt
{
  NSMutableDictionary<NSNumber *, GameObject *> *res = [NSMutableDictionary dictionary];
  uint64_t t0 = [self currentTime];
  uint64_t t1 = t0;
  
  while (t1 - t0 < dt) {
    NSArray<NSNumber *> *data = [self readCurrentRowAndAdvancing:YES];
    t1 = data[1].unsignedIntegerValue;
    GameObject *obj = [res objectForKey:data[0]];
    if (!obj) {
      obj = [[GameObject alloc] init];
      [obj setSize:3.0];
      [obj setColor:[NSColor redColor]];
      [res setObject:obj forKey:data[0]];
    }
    [obj setX:data[2].doubleValue];
    [obj setY:data[3].doubleValue];
    [obj setZ:data[4].doubleValue];
  }
  
  return res;
}


@end
