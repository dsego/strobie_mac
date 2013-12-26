/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import <Cocoa/Cocoa.h>
#import "alerts.h"


void audioDeviceErrorAlert(void) {

  NSAlert *alert = [[NSAlert alloc] init];
  alert.messageText = @"Audio device error";
  alert.informativeText = @"Current audio settings aren't working. "
                          "Please try a different sample rate and buffer size, "
                          "or choose a different audio device.";
  [alert runModal];

}


int trialExpiredAlert(void) {

  return [[NSAlert
    alertWithMessageText: @"Your trial has ended."
    defaultButton: @"Buy"
    alternateButton: @"Close"
    otherButton: nil
    informativeTextWithFormat: @"If you would like to continue using Strobie, please buy the full version."
  ] runModal];

}


int alertDaysLeft(int days) {

  NSString *info;

  if (days <= 1) {
    info = @"This is the last day of your trial.";
  }
  else {
    info = [NSString stringWithFormat: @"You have %d days of trial left.", days];
  }

  NSAlert *alert = [[NSAlert alloc] init];
  alert.messageText = @"Thank you for trying out Strobie!";
  alert.informativeText = info;
  [alert addButtonWithTitle: @"Continue Trial"];
  [alert addButtonWithTitle: @"Buy"];

  return [alert runModal];

}
