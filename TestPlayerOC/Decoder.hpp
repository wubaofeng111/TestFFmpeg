//
//  Decoder.hpp
//  TestPlayerOC
//
//  Created by friday on 2019/6/28.
//  Copyright © 2019 friday. All rights reserved.
//

#ifndef Decoder_hpp
#define Decoder_hpp

#include <stdio.h>
#include <string>
#ifdef __cplusplus
extern "C"
{
#endif
#include "avformat.h"
#include "avutil.h"
#include "dict.h"
#include "avcodec.h"
#include "imgutils.h"
#include "swscale.h"
#ifdef __cplusplus
};
#endif

class BFDecoder
{
private:
    AVFormatContext *pAVFormatContext;
    AVInputFormat   *pAVInputFormat;
    AVDictionary    *pAVDictionary;
    AVCodecContext  *pAVCodecContextVideo;
    AVCodecContext  *pAVCodecContextAudio;
    int              videoIndex;
    int              audioIndex;
    AVFrame         *pAVFrameVideo;
    AVFrame         *pAVFrameVswsFrame;
    AVFrame         *pAVFrameAudio;
    
    bool FindCodecFormate();
    AVCodecContext  *FindCodecCtx(int index);
    
    
public:
    BFDecoder();
    ~BFDecoder();
    AVDictionary    *FindMetaData();
    bool OpenURL(char *url);
};

#endif /* Decoder_hpp */
