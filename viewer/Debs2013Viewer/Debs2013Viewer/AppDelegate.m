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
#import "Matrix.h"


const NSTimeInterval frameRate = 1.0/60.0;


@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet GameStateView *gsvw;
@property (weak) IBOutlet NSSlider *timeSlider;
@property (weak) IBOutlet NSTextField *timeTextField;

@property (nonatomic) BOOL viewIn3D;

/* angle from the ground to the camera */
@property (nonatomic) double cameraAngle;
/* rotation angle of the camera along its own axis */
@property (nonatomic) double cameraRotation;

@end


@implementation AppDelegate {
  GameDataStream *gds;
  NSTimer *playbackTimer;
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  [self.gsvw setFieldRect:NSMakeRect(0, -33960, 52483, 33960+33965)];
  self.cameraAngle = 60.0 * M_PI / 180.0;
  self.cameraRotation = 0.0;
  self.viewIn3D = YES;
  
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


- (void)setViewIn3D:(BOOL)viewIn3D
{
  _viewIn3D = viewIn3D;
  [self setupProjectionMatrix];
}


- (void)setCameraAngle:(double)cameraAngle
{
  _cameraAngle = cameraAngle;
  [self setupProjectionMatrix];
}


- (void)setCameraRotation:(double)cameraRotation
{
  _cameraRotation = cameraRotation;
  [self setupProjectionMatrix];
}


- (IBAction)setExtraRadius:(id)sender
{
  double v = [sender doubleValue];
  [self.gsvw setExtraRadius:v];
}


- (void)setupProjectionMatrix
{
  Matrix *wvpm;
  
  if (!self.viewIn3D) {
    double scaleFactor = 1.0/((33960.0+33965.0+10000)/2.0);
    wvpm = [[Matrix scalingByX:scaleFactor y:scaleFactor z:scaleFactor] multiply:
        [Matrix translationOfX:(-52483.0)/2.0 y:0 z:0]];
    
  } else {
    double cameraDistanceFromCenterOfField = 70000;
    double centerOfFieldX = 52483.0 / 2.0;
    double cameraX = cameraDistanceFromCenterOfField * cos(self.cameraAngle) + centerOfFieldX;
    double cameraZ = cameraDistanceFromCenterOfField * sin(self.cameraAngle);
    
    Matrix *viewm =
      [[[Matrix rotationAroundZAxisOf:-(self.cameraRotation + M_PI / 2.0)] multiply:
        [Matrix rotationAroundYAxisOf:-(M_PI / 2.0 - self.cameraAngle)]] multiply:
            [Matrix translationOfX:-cameraX y:0 z:-cameraZ]];
    wvpm = [[Matrix perspectiveProjectionWithFovY:M_PI/3.0 nearPlane:1000 farPlane:cameraDistanceFromCenterOfField aspect:1.0] multiply:viewm];
  }
  
  [self.gsvw setWorldViewProjectionMatrix:wvpm];
}


- (void)nextFrame
{
  NSDictionary *data = [gds advanceByTimeInterval:1000000000000 * frameRate];
  [self.gsvw clearObjects];
  [self.gsvw updateObjects:data];
  [self.timeSlider setDoubleValue:[gds currentTime]];
  [self.timeTextField setIntegerValue:[self->gds currentTime]];
}


- (IBAction)scrub:(id)sender
{
  [self pause];
  [gds skipToTime:self.timeSlider.integerValue - (1000000000000 * frameRate)];
  NSDictionary *data = [gds advanceByTimeInterval:(1000000000000 * frameRate)];
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
    playbackTimer = [NSTimer scheduledTimerWithTimeInterval:frameRate repeats:YES block:^(NSTimer *tmr){
      AppDelegate *strongSelf = weakSelf;
      [strongSelf nextFrame];
    }];
  } else {
    [self pause];
  }
}


@end
