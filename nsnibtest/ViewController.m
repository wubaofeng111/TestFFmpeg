//
//  ViewController.m
//  nsnibtest
//
//  Created by friday on 2019/4/23.
//  Copyright © 2019 friday. All rights reserved.
//

#import "ViewController.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSNib *nib;
    
    
    [nib instantiateWithOwner:nil topLevelObjects:nil];
    
    BOOL isEnglish;
    NSString *key;
    NSString *des = nil;
//
//    [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"RDVEUISDK.bundle/%@", isEnglish ? @"en" : @"zh-Hans"] ofType:@"lproj"]
//
//    [NSBundle bundleWithPath:]
//
//    [ localizedStringForKey:(key) value:des table:@"RDVEUISDK_Localizable"];
//
    NSBundle *bundle =  [NSBundle bundleWithPath: [[NSBundle  mainBundle] pathForResource:@"zh-Hans.lproj" ofType:nil]];
    NSLog(@"%@",bundle);
    NSString *string = [[NSBundle mainBundle] localizedStringForKey:@"wbf" value:nil table:@"chMainString"];
    NSLog(@"%@",string);
    string = [[NSBundle mainBundle] localizedStringForKey:@"wbf" value:nil table:@"enMainString"];
    NSLog(@"%@",string);
    NSBundle *deskBundle = [NSBundle bundleWithPath:NSHomeDirectory()];
    
    string = [deskBundle localizedStringForKey:@"wbf" value:nil table:@"chMainString"];
    NSLog(@"%@",string);
    
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
