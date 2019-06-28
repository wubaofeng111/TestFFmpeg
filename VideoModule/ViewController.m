//
//  ViewController.m
//  VideoModule
//
//  Created by friday on 2019/4/28.
//  Copyright © 2019 friday. All rights reserved.
//

#import "ViewController.h"
#import "GLView.h"
@interface ViewController ()
{
    OpenglView *glView;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    glView = [[OpenglView alloc]initWithFrame:self.view.bounds];
    [self.view addSubview:glView];
    // Do any additional setup after loading the view.
}

-(UIStatusBarStyle)preferredStatusBarStyle
{
    return UIStatusBarStyleDefault;
}

@end
