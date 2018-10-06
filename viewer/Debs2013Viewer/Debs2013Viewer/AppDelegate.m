//
//  AppDelegate.m
//  Debs2013Viewer
//
//  Created by Daniele Cattaneo on 05/10/18.
//  Copyright © 2018 danielecattaneo. All rights reserved.
//

#import "AppDelegate.h"
#import "GameStateView.h"
#import "GameDataStream.h"


@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet GameStateView *gsvw;
@property (weak) IBOutlet NSSlider *timeSlider;
@property (weak) IBOutlet NSTextField *timeTextField;

@end


@implementation AppDelegate {
  GameDataStream *gds;
  NSTimer *playbackTimer;
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  double wvpm_topdown[] = {
    1.0, 0.0, 0.0, (-52483.0)/2.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, (33960.0+33965.0+10000)/2.0
  };
  [self.gsvw setFieldRect:NSMakeRect(0, -33960, 52483, 33960+33965)];
  [self.gsvw setWorldViewProjectionMatrix:wvpm_topdown];
  
  NSOpenPanel *fop = [NSOpenPanel openPanel];
  [fop setAllowedFileTypes:@[@"csv"]];
  [fop beginSheetModalForWindow:self.window completionHandler:^(NSModalResponse resp){
    if (resp == NSModalResponseCancel) {
      [NSApp terminate:nil];
    }
    self->gds = [[GameDataStream alloc] initWithDataFile:fop.URL];
    [self.timeSlider setMinValue:[self->gds startTime]];
    [self.timeSlider setMaxValue:[self->gds endTime]];
    [self.timeTextField setIntegerValue:[self->gds currentTime]];
  }];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
  [playbackTimer invalidate];
}


- (void)nextFrame
{
  NSDictionary *data = [gds advanceByTimeInterval:1000000000000/60];
  [self.gsvw clearObjects];
  [self.gsvw updateObjects:data];
  [self.timeSlider setDoubleValue:[gds currentTime]];
  [self.timeTextField setIntegerValue:[self->gds currentTime]];
}


- (IBAction)scrub:(id)sender
{
  [self pause];
  [gds skipToTime:self.timeSlider.integerValue - 1000000000000/60];
  NSDictionary *data = [gds advanceByTimeInterval:1000000000000/60];
  [self.gsvw clearObjects];
  [self.gsvw updateObjects:data];
  [self.timeTextField setIntegerValue:[self->gds currentTime]];
}


- (void)pause
{
  if (playbackTimer) {
    [playbackTimer invalidate];
    playbackTimer = nil;
  }
}


- (IBAction)playPause:(id)sender
{
  if (!gds)
    return;
  if (!playbackTimer) {
    __weak AppDelegate *weakSelf = self;
    playbackTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0 repeats:YES block:^(NSTimer *tmr){
      AppDelegate *strongSelf = weakSelf;
      [strongSelf nextFrame];
    }];
  } else {
    [self pause];
  }
}


@end
