/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <Foundation/Foundation.h>
#import "MinLAppPal.h"
#import <stdio.h>
#import <stdlib.h>
#import <stdarg.h>
#import <sys/stat.h>
#import <iostream>
#import <fstream>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "MinLAppDefine.h"

using std::endl;
using namespace Csm;
using namespace std;
using namespace MinLAppDefine;

double MinLAppPal::s_currentFrame = 0.0;
double MinLAppPal::s_lastFrame = 0.0;
double MinLAppPal::s_deltaTime = 0.0;

csmByte* MinLAppPal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    int path_i = static_cast<int>(filePath.find_last_of("/")+1);
    int ext_i = static_cast<int>(filePath.find_last_of("."));
    std::string pathname = filePath.substr(0,path_i);
    std::string extname = filePath.substr(ext_i,filePath.size()-ext_i);
    std::string filename = filePath.substr(path_i,ext_i-path_i);
    NSString* castFilePath = [[NSBundle mainBundle]
                              pathForResource:[NSString stringWithUTF8String:filename.c_str()]
                              ofType:[NSString stringWithUTF8String:extname.c_str()]
                              inDirectory:[NSString stringWithUTF8String:pathname.c_str()]];

    NSError *errorMsg = nil;
    NSData *data = [NSData dataWithContentsOfFile:castFilePath];

    if (data == nil)
    {
        NSLog(@"Failed to read file, error %@", errorMsg);
        return NULL;
    }

    NSUInteger len = [data length];
    Byte *byteData = (Byte*)malloc(len);
    memcpy(byteData, [data bytes], len);

    *outSize = static_cast<Csm::csmSizeInt>(len);
    return static_cast<Csm::csmByte*>(byteData);
}

void MinLAppPal::ReleaseBytes(csmByte* byteData)
{
    free(byteData);
}

void MinLAppPal::UpdateTime()
{
    NSDate *now = [NSDate date];
    double unixtime = [now timeIntervalSince1970];
    s_currentFrame = unixtime;
    s_deltaTime = s_currentFrame - s_lastFrame;
    s_lastFrame = s_currentFrame;
}

void MinLAppPal::PrintLog(const csmChar* format, ...)
{
    va_list args;
    Csm::csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args); // 標準出力でレンダリング;
    NSLog(@"%@",[NSString stringWithCString:buf encoding:NSUTF8StringEncoding]);
    va_end(args);
}

void MinLAppPal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}
