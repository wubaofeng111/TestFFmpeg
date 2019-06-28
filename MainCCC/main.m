//
//  main.m
//  MainCCC
//
//  Created by friday on 2019/4/26.
//  Copyright © 2019 friday. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "avfilter.h"
#import "avdevice.h"
#import <AudioToolbox/AudioToolbox.h>

typedef struct Handle{
    NSInputStream* inputSteam;
}LYPlayer;


static OSStatus PlayCallback(void *inRefCon,
                             AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp,
                             UInt32 inBusNumber,
                             UInt32 inNumberFrames,
                             AudioBufferList *ioData)
{
    LYPlayer *player = (LYPlayer *)inRefCon;
    
    ioData->mBuffers[0].mDataByteSize = (UInt32)[player->inputSteam read:ioData->mBuffers[0].mData
                                                               maxLength:(NSInteger)ioData->mBuffers[0].mDataByteSize];
    NSLog(@"out size: %d", ioData->mBuffers[0].mDataByteSize);
    
    if (ioData->mBuffers[0].mDataByteSize <= 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        });
    }
    return noErr;
}





const uint32 CONST_BUFFER_SIZE = 0x10000;




int main(int argc, const char * argv[]) {
    @autoreleasepool {
        CVImageBufferRef
        
        FILE *file = fopen("/Users/friday/Desktop/output.pcm", "rw");
        
        if(!file) return -1;
        fseek(file,0L,SEEK_END);
        int size=ftell(file);
        
        fseek(file, 0L, SEEK_SET);
        int value = 0 ,value2 = 0;
//        for (int i = 0; i < size/4; i++) {
//            fread(&value, 4, 1, file);
//            fread(&value2, 4, 1, file);
//            printf("%d %d\n",value ,value2);
//        }
        
        
        
        
        AudioUnit audioUnit;
        
      
        
        AudioComponentDescription audioDesc;
        audioDesc.componentType = kAudioUnitType_Output;
        audioDesc.componentSubType = kAudioUnitSubType_SystemOutput;
        audioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
        audioDesc.componentFlags = 0;
        audioDesc.componentFlagsMask = 0;
        
        AudioComponent inputComponent = AudioComponentFindNext(NULL, &audioDesc);
        AudioComponentInstanceNew(inputComponent, &audioUnit);
        
        // buffer
        AudioBufferList* buffList = (AudioBufferList *)malloc(sizeof(AudioBufferList));
        buffList->mNumberBuffers = 1;
        buffList->mBuffers[0].mNumberChannels = 1;
        buffList->mBuffers[0].mDataByteSize = CONST_BUFFER_SIZE;
        buffList->mBuffers[0].mData = malloc(CONST_BUFFER_SIZE);
        OSStatus status;
        UInt32 flag = 1;
        if (flag) {
            status = AudioUnitSetProperty(audioUnit,
                                          kAudioOutputUnitProperty_EnableIO,
                                          kAudioUnitScope_Output,
                                          0,
                                          &flag,
                                          sizeof(flag));
        }
        
//        ffmpeg -i out.aac -codec:a pcm_f32le -ar 48000 -ac 2 -f f32le output.pcm
        
        AudioStreamBasicDescription outputFormat;
        memset(&outputFormat, 0, sizeof(outputFormat));
        outputFormat.mSampleRate       = 22050; // 采样率
        outputFormat.mFormatID         = kAudioFormatLinearPCM; // PCM格式
        outputFormat.mFormatFlags      = kLinearPCMFormatFlagIsFloat; // 整形
        outputFormat.mFramesPerPacket  = 1; // 每帧只有1个packet
        outputFormat.mChannelsPerFrame = 2; // 声道数
        outputFormat.mBytesPerFrame    = 16; // 每帧只有2个byte 声道*位深*Packet数
        outputFormat.mBytesPerPacket   = 16; // 每个Packet只有2个byte
        outputFormat.mBitsPerChannel   = 32; // 位深
        
        // 48000
        //   package 1 frame  1
        //   changes 2 bytes  2
        
        
        
        LYPlayer *self = (LYPlayer*)malloc(sizeof(LYPlayer));
        
        self->inputSteam = [NSInputStream inputStreamWithURL:[NSURL fileURLWithPath:@"/Users/friday/Desktop/output.pcm"]];
        [self->inputSteam open];
        
        AURenderCallbackStruct playCallback;
        playCallback.inputProc = PlayCallback;
        playCallback.inputProcRefCon = self;
        AudioUnitSetProperty(audioUnit,
                             kAudioUnitProperty_SetRenderCallback,
                             kAudioUnitScope_Input,
                             0,
                             &playCallback,
                             sizeof(playCallback));
        
        
        
        
        OSStatus result = AudioUnitInitialize(audioUnit);
        AudioOutputUnitStart(audioUnit);
        
        
        [[NSRunLoop currentRunLoop] run];
        
        return 0;
        
        avdevice_register_all();
        
        
        NSURL *outUrl = [NSURL fileURLWithPath:@"/Users/friday/Desktop/5.mov"];
        
        NSURL *url = [NSURL fileURLWithPath:@"/Users/friday/Desktop/IMG_0014.mov"];
        
        AVAsset *avAsset = [AVAsset assetWithURL:url];
        AVAssetTrack *videoTrack = [[avAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
        AVMutableComposition* mixComposition = [[AVMutableComposition alloc] init];
        
        AVMutableCompositionTrack *firstTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeVideo
                                                                            preferredTrackID:kCMPersistentTrackID_Invalid];
        [firstTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, avAsset.duration) ofTrack:videoTrack atTime:kCMTimeZero error:nil];
        
        
        AVAssetTrack *audiotrack = [[avAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
        if(audiotrack!=nil){
            AVMutableCompositionTrack *AudioTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
            [AudioTrack insertTimeRange:audiotrack.timeRange  ofTrack:audiotrack atTime:kCMTimeZero error:nil];
        }
        
        AVMutableVideoCompositionInstruction * MainInstruction = [AVMutableVideoCompositionInstruction videoCompositionInstruction];
        MainInstruction.timeRange = CMTimeRangeMake(kCMTimeZero, avAsset.duration);
        
        AVMutableVideoCompositionLayerInstruction *FirstlayerInstruction = [AVMutableVideoCompositionLayerInstruction videoCompositionLayerInstructionWithAssetTrack:firstTrack];
        
         CGFloat FirstAssetScaleToFitRatio = 1;
        
        [FirstlayerInstruction setOpacity:0.0 atTime:avAsset.duration];
        
        MainInstruction.layerInstructions = [NSArray arrayWithObjects:FirstlayerInstruction,nil];;
        
        
        AVMutableVideoComposition *MainCompositionInst = [AVMutableVideoComposition videoComposition];
        MainCompositionInst.instructions = [NSArray arrayWithObject:MainInstruction];
        MainCompositionInst.frameDuration = CMTimeMake(1, 30);
        MainCompositionInst.renderSize = CGSizeMake(videoTrack.naturalSize.width, videoTrack.naturalSize.height);
        
       AVAssetExportSession* _export = [[AVAssetExportSession alloc] initWithAsset:mixComposition presetName:AVAssetExportPresetHighestQuality];
        _export.outputURL=outUrl;
        _export.timeRange = CMTimeRangeMake(CMTimeMake(0, 0), CMTimeMake(1, 30));;
        _export.outputFileType = AVFileTypeQuickTimeMovie;
        _export.videoComposition = MainCompositionInst;
        _export.shouldOptimizeForNetworkUse = YES;
        
        [_export exportAsynchronouslyWithCompletionHandler:^{
            NSLog(@"1234");
        }];
        
        [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}
