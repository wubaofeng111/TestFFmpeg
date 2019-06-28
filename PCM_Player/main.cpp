//
//  main.m
//  PCM_Player
//
//  Created by friday on 2019/4/30.
//  Copyright © 2019 friday. All rights reserved.
//


#include <AudioToolbox/AudioToolbox.h>


#define N_OC 0


typedef struct LYPlayer{
#if N_OC
     FILE* inputSteam;
#else
//    NSInputStream *inputSteam;
#endif
   
    
//    void close();
}LYPlayer;

const UInt32 CONST_BUFFER_SIZE = 0x10000;



//AudioStreamBasicDescription clientDesc;
//clientDesc.mSampleRate = fileDesc.mSampleRate;
//clientDesc.mFormatID = kAudioFormatLinearPCM;
//clientDesc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
//clientDesc.mReserved = 0;
//clientDesc.mChannelsPerFrame = 1; //2
//clientDesc.mBitsPerChannel = 16;
//clientDesc.mFramesPerPacket = 1;
//clientDesc.mBytesPerFrame = clientDesc.mChannelsPerFrame * clientDesc.mBitsPerChannel / 8;
//clientDesc.mBytesPerPacket = clientDesc.mBytesPerFrame;


static OSStatus PlayCallback(void *inRefCon,
                             AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp,
                             UInt32 inBusNumber,
                             UInt32 inNumberFrames,
                             AudioBufferList *ioData)
{
    LYPlayer *player = (LYPlayer *)inRefCon;
    
    
#if N_OC
    fread(&(ioData->mBuffers[0].mData), 2048, 1, player->inputSteam);
    ioData->mBuffers[0].mDataByteSize = 2048;
    
    printf("out size: %d %d\n", ioData->mBuffers[0].mDataByteSize,((int*)ioData->mBuffers[0].mData)[0]);
#else
//    ioData->mBuffers[0].mDataByteSize = (UInt32)[player->inputSteam read:ioData->mBuffers[0].mData
//                                                               maxLength:(NSInteger)ioData->mBuffers[0].mDataByteSize];
//    NSLog(@"out size: %d %d", ioData->mBuffers[0].mDataByteSize,((int*)ioData->mBuffers[0].mData)[0]);
#endif
    
    
    if (ioData->mBuffers[0].mDataByteSize <= 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
//            player->close();
        });
    }
    return noErr;
}

//f1    FILE *    0x7fffab83f030    0x00007fffab83f030
//f1    FILE *    0x7fffab83f030    0x00007fffab83f030
//f1    FILE *    0x7fffab83f030    0x00007fffab83f030
int main(int argc, const char * argv[]) {
    
    FILE *f1 = fopen("/Users/friday/Desktop/music.mov", "rw");
    printf("is first file");
    
    
    
    
//    FILE *file = fopen("/Users/friday/Desktop/output.pcm", "rw");
//
//    if(!file) return -1;
//    fseek(file,0L,SEEK_END);
//    int size=ftell(file);
//
//    fseek(file, 0L, SEEK_SET);
//    int value = 0 ,value2 = 0;
//    //        for (int i = 0; i < size/4; i++) {
//    //            fread(&value, 4, 1, file);
//    //            fread(&value2, 4, 1, file);
//    //            printf("%d %d\n",value ,value2);
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
    outputFormat.mSampleRate       = 48000; // 采样率
    outputFormat.mFormatID         = kAudioFormatLinearPCM; // PCM格式
    outputFormat.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger; // 整形
    outputFormat.mFramesPerPacket  = 1; // 每帧只有1个packet
    outputFormat.mChannelsPerFrame = 1; // 声道数
    outputFormat.mBytesPerFrame    = 2; // 每帧只有2个byte 声道*位深*Packet数
    outputFormat.mBytesPerPacket   = 2; // 每个Packet只有2个byte
    outputFormat.mBitsPerChannel   = 16; // 位深

    // 48000
    //   package 1 frame  1
    //   changes 2 bytes  2
    
    
    
    LYPlayer *self = (LYPlayer*)malloc(sizeof(LYPlayer));
#if N_OC
    self->inputSteam = fopen("/Users/friday/Desktop/output.pcm", "ab+");
#else
//    self->inputSteam = [NSInputStream inputStreamWithURL:[NSURL fileURLWithPath: @"/Users/friday/Desktop/output.pcm"]];
//    [self->inputSteam open];
#endif
    
    AURenderCallbackStruct playCallback;
    playCallback.inputProc = PlayCallback;
    playCallback.inputProcRefCon = self;
    
//    AudioUnitSetProperty(                AudioUnit                inUnit,
//                         AudioUnitPropertyID        inID,
//                         AudioUnitScope            inScope,
//                         AudioUnitElement        inElement,
//                         const void * __nullable    inData,===
//                         UInt32                    inDataSize)
    AudioUnitSetProperty(audioUnit,
                         kAudioUnitProperty_SetRenderCallback,
                         kAudioUnitScope_Input,
                         0,
                         &playCallback,
                         sizeof(playCallback));
    
    
    
    
    OSStatus result = AudioUnitInitialize(audioUnit);
    AudioOutputUnitStart(audioUnit);
    while (1) {
        sleep(10);
    }

    return 0;
}
