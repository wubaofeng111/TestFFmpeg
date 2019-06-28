//
//  main.m
//  viewTest
//
//  Created by friday on 2019/4/23.
//  Copyright © 2019 friday. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <VideoToolbox/VideoToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <ReplayKit/ReplayKit.h>
#import <OpenGLES/ES3/gl.h>
#import <GLKit/GLKit.h>

typedef struct{
    GLKVector3 positionCoords;
}sceneVertex;

//三角形的三个顶点
static const sceneVertex vertices[] = {
    {{-0.5f,-0.5f,0.0}},
    {{0.5f,-0.5f,0.0}},
    {{-0.5f,0.5f,0.0}},
    {{0.5f,0.5f,0.0}},
};

@interface BFViewController : GLKViewController<UIApplicationDelegate>
{
    UIWindow *window;
    GLint vertextBufferID;
    GLuint                  _program;
}
@property(nonatomic,strong)GLKBaseEffect *baseEffect;
@end

@implementation BFViewController

-(BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    window = [[UIWindow alloc]initWithFrame:[UIScreen mainScreen].bounds];
    [window makeKeyAndVisible];
    window.rootViewController = self;
    return YES;
}

-(GLuint)CompileShader:(char *)shaderString b:(GLenum) shaderType
{
    if (shaderString == NULL) {
        return 0;
    }else{
        
    }
    
    GLuint shaderHandle = glCreateShader(shaderType);
    size_t shaderString_Length = strlen(shaderString);
    glShaderSource(shaderHandle, 1, (const GLchar *const *)&shaderString, (const GLint *)&shaderString_Length);
    //    glShaderSource(shaderHandle, 1, &shaderString, &shaderString_Length);
    
    glCompileShader(shaderHandle);
    
    GLint compileSuccess = GL_FALSE;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        printf("%s\n",messages);
        //        exit(0);
    }
    
    return shaderHandle;
}
enum AttribEnum
{
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
    ATTRIB_COLOR,
};

-(void)loadShader
{
    const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
    printf("OpenGL实现的版本号：%s\n",OpenGLVersion );
    
    
    FILE *filefs = fopen("/Users/friday/Documents/LearnExample/TestFFmpeg/viewTest/shader.vsh", "ab+");
    fseek(filefs, 0, SEEK_END);
    size_t filesize = ftell(filefs);
    
    
    char *fsStr = (char*)malloc(filesize);
    fseek(filefs, 0, SEEK_SET);
    fread(fsStr, filesize, 1, filefs);
    
    printf("%s",fsStr);
    
    GLuint vertexShader = [self CompileShader:fsStr b:GL_VERTEX_SHADER];
    
    free(fsStr);
    
    fsStr = NULL;
    
    FILE *filevs = fopen("/Users/friday/Documents/LearnExample/TestFFmpeg/viewTest/shader.fsh", "ab+");
    fseek(filevs, 0, SEEK_END);
    filesize = ftell(filevs);
    fsStr = (char*)malloc(filesize);
    fseek(filevs, 0, SEEK_SET);
    fread(fsStr, filesize, 1, filevs);
    
    printf("%s",fsStr);
    
    
    GLuint fragmentShader = [self CompileShader:fsStr b:GL_FRAGMENT_SHADER];
    _program = glCreateProgram();
    
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    
    glUseProgram(_program);
    
    glBindAttribLocation(_program, ATTRIB_VERTEX, "vPosition");
    glBindAttribLocation(_program, ATTRIB_TEXTURE, "fragColor");
    glLinkProgram(_program);
    
    GLint linkSuccess;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
    
    if(linkSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog(_program, sizeof(messages), 0, &messages[0]);
        printf("link error:%s\n",messages);
    }
    
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    NSLog(@"%@", NSHomeDirectory());

    NSBundle *mainBundle = [NSBundle bundleWithPath:[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/main.bundle"]];



    UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
    [btn setTitle:@"Main" forState:UIControlStateNormal];
    [btn setImage:[UIImage imageNamed:[mainBundle pathForResource:@"main" ofType:@"png"]] forState:UIControlStateNormal];
    btn.frame = CGRectMake(40, 40, 120, 60);

    [self.view addSubview:btn];

    GLKView *view  = (GLKView *)self.view;
    NSAssert([view isKindOfClass:[GLKView class]], @"ViewController's View is Not A GLKView");
    //创建OpenGL ES2.0上下文
    view.context = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];

    //设置当前上下文
    [EAGLContext setCurrentContext:view.context];

//    self.baseEffect = [[GLKBaseEffect alloc]init];
//    self.baseEffect.useConstantColor = GL_TRUE;
////    self.baseEffect.constantColor = GLKVector4Make(1.0f, 1.0f, 1.0f, 1.0f);
//
//    //设置背景色
//    glClearColor(0.0f,0.0f,0.0f,1.0f);
//
//
//    //Gen Bind Initial Memory
//
//    glGenBuffers(1, &vertextBufferID);
//    glBindBuffer(GL_ARRAY_BUFFER, vertextBufferID); //绑定指定标识符的缓存为当前缓存
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    [self loadShader];

}



- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
//    GLuint _renderBuffer;
//    GLuint _frameBuffer;
//
//    glGenRenderbuffers(1, &_renderBuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
//
//    [view.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)view.layer];
//
//    glGenFramebuffers(1, &_frameBuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
//
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    
}


-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    NSLog(@"is loaded");
}

@end



int main(int argc, char * argv[]) {
    @autoreleasepool {
        
       return UIApplicationMain(argc, argv, nil, @"BFViewController");
        
        
      
        
//        [[NSRunLoop currentRunLoop] run];
        
        
        
//        NSString *localPath =  [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
//
//        NSURL *outUrl = [NSURL fileURLWithPath:[localPath stringByAppendingPathComponent:@"1.mov"]];
//
//        NSURL *url = [NSURL fileURLWithPath:[localPath stringByAppendingPathComponent:@"IMG_0014.mov"]];
//
//        AVAsset *avAsset = [AVAsset assetWithURL:url];
//        AVAssetTrack *videoTrack = [[avAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
//        AVMutableComposition* mixComposition = [[AVMutableComposition alloc] init];
//
//        AVMutableCompositionTrack *firstTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeVideo
//                                                                            preferredTrackID:kCMPersistentTrackID_Invalid];
//        [firstTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, avAsset.duration) ofTrack:videoTrack atTime:kCMTimeZero error:nil];
//
//
//        AVAssetTrack *audiotrack = [[avAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
//        if(audiotrack!=nil){
//            AVMutableCompositionTrack *AudioTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
//            [AudioTrack insertTimeRange:audiotrack.timeRange  ofTrack:audiotrack atTime:kCMTimeZero error:nil];
//        }
//
//        AVMutableVideoCompositionInstruction * MainInstruction = [AVMutableVideoCompositionInstruction videoCompositionInstruction];
//        MainInstruction.timeRange = CMTimeRangeMake(kCMTimeZero, avAsset.duration);
//
//        AVMutableVideoCompositionLayerInstruction *FirstlayerInstruction = [AVMutableVideoCompositionLayerInstruction videoCompositionLayerInstructionWithAssetTrack:firstTrack];
//
//        CGFloat FirstAssetScaleToFitRatio = 1;
//
//        [FirstlayerInstruction setOpacity:0.0 atTime:avAsset.duration];
//
//        MainInstruction.layerInstructions = [NSArray arrayWithObjects:FirstlayerInstruction,nil];;
//
//
//        AVMutableVideoComposition *MainCompositionInst = [AVMutableVideoComposition videoComposition];
//        MainCompositionInst.instructions = [NSArray arrayWithObject:MainInstruction];
//        MainCompositionInst.frameDuration = CMTimeMake(1, 30);
//        MainCompositionInst.renderSize = CGSizeMake(videoTrack.naturalSize.width, videoTrack.naturalSize.height);
//
//        AVAssetExportSession* _export = [[AVAssetExportSession alloc] initWithAsset:mixComposition presetName:AVAssetExportPresetHighestQuality];
//        _export.outputURL=outUrl;
//        _export.timeRange = CMTimeRangeMake(CMTimeMake(0, 0), CMTimeMake(1, 30));;
//        _export.outputFileType = AVFileTypeQuickTimeMovie;
//        _export.videoComposition = MainCompositionInst;
//        _export.shouldOptimizeForNetworkUse = YES;
//
//        [_export exportAsynchronouslyWithCompletionHandler:^{
//            NSLog(@"1234");
//        }];
//
//        [[NSRunLoop currentRunLoop] run];
        return 0;//UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
