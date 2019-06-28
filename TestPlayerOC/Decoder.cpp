//
//  Decoder.cpp
//  TestPlayerOC
//
//  Created by friday on 2019/6/28.
//  Copyright © 2019 friday. All rights reserved.
//

#include "Decoder.hpp"


BFDecoder::BFDecoder()
{
    pAVFormatContext = NULL;
    
    av_register_all();
}
BFDecoder::~BFDecoder()
{
    
}

bool BFDecoder::FindCodecFormate()
{
    AVMediaType type;
    for(int i = 0;i<pAVFormatContext->nb_streams;i++)
    {
        type = pAVFormatContext->streams[i]->codecpar->codec_type;
        if (type == AVMEDIA_TYPE_AUDIO) {
            AVCodecContext *lpAVCodecContext = FindCodecCtx(i);
            if (lpAVCodecContext != NULL) {
                pAVCodecContextAudio = lpAVCodecContext;
                audioIndex = i;
            }
            continue;
        }
        
        if (type == AVMEDIA_TYPE_VIDEO) {
            int disposition = pAVFormatContext->streams[i]->disposition;
            if ((disposition&AV_DISPOSITION_ATTACHED_PIC) == 0) {
                AVCodecContext *lpAVCodecContext = FindCodecCtx(i);
                if (lpAVCodecContext != NULL) {
                    pAVCodecContextVideo = lpAVCodecContext;
                    videoIndex = i;
                }
            }
            continue;
        }
    }
    return true;
}

AVCodecContext *BFDecoder::FindCodecCtx(int index)
{
    AVCodecParameters *params = pAVFormatContext->streams[index]->codecpar;
    AVCodec           *codec  = avcodec_find_decoder(params->codec_id);
    if (codec == NULL) {
        return NULL;
    }
    AVCodecContext *ctx = avcodec_alloc_context3(codec);
    if (ctx == NULL) {
        return NULL;
    }
    
    int ret = avcodec_parameters_to_context(ctx, params);
    if (ret < 0) {
        avcodec_free_context(&ctx);
        return NULL;
    }
    
    ret = avcodec_open2(ctx, codec, NULL);
    if (ret < 0) {
        avcodec_free_context(&ctx);
        return NULL;
    }
    
    return ctx;
}

bool BFDecoder::OpenURL(char *url)
{
    int yet = avformat_open_input(&pAVFormatContext, url, NULL, NULL);
    if (yet != 0) {
        printf("打开失败\n");
        perror("打开文件失败");
        return false;
    }
    
    yet = avformat_find_stream_info(pAVFormatContext, NULL);
    if (yet != 0) {
        perror("提取流信息失败");
        return false;
    }
    
    bool isFind = FindCodecFormate();
    if (isFind == false) {
        perror("提取音频格式错误");
        return false;
    }
    bool isYUV = false;
    struct SwsContext *pSwsContext = NULL;

    if (pAVCodecContextVideo->pix_fmt != AV_PIX_FMT_NONE) {
        pAVFrameVideo = av_frame_alloc();
        isYUV = (pAVCodecContextVideo->pix_fmt == AV_PIX_FMT_YUV420P||pAVCodecContextVideo->pix_fmt == AV_PIX_FMT_YUVJ420P);
        if (!isYUV) {
            pAVFrameVswsFrame = av_frame_alloc();
            yet = av_image_alloc(pAVFrameVswsFrame->data,pAVFrameVswsFrame->linesize,pAVCodecContextVideo->width,pAVCodecContextVideo->height,AV_PIX_FMT_RGB24,1);
            pSwsContext = sws_getContext(pAVCodecContextVideo->width, pAVCodecContextVideo->height, pAVCodecContextVideo->pix_fmt, pAVCodecContextVideo->width, pAVCodecContextVideo->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
        }
        
    }
    printf("完成！");
    return true;
}



AVDictionary * BFDecoder::FindMetaData()
{
    AVDictionary *metadata = pAVFormatContext->metadata;
    AVDictionaryEntry *entry = av_dict_get(metadata, "", NULL, AV_DICT_IGNORE_SUFFIX);
    int a = 0;
    printf("\n{\n");
    while (entry!=NULL) {
        printf("\t%s:%s\n",entry->key,entry->value);
        if (a++ > 20) {
            break;
        }
    }
    printf("}");
    return metadata;
}
