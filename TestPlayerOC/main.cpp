//
//  main.m
//  TestPlayerOC
//
//  Created by friday on 2019/6/28.
//  Copyright © 2019 friday. All rights reserved.
//
#ifdef __cplusplus
extern "C"
{
#endif
#include "avformat.h"
#include "swscale.h"
#include "imgutils.h"
#include "eval.h"
#include "display.h"
#include "swresample.h"
#ifdef __cplusplus
};
#endif
#include "Decoder.hpp"
static BFDecoder *decoder = NULL;

int main(int argc, const char * argv[]) {
    decoder = new BFDecoder();
    decoder->OpenURL("/Users/friday/Documents/LearnExample/TestFFmpeg/TestPlayerOC/bbq.mov");
    decoder->FindMetaData();
    return 0;
}
