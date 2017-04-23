//
//  NArt.m
//  NArt-ios
//
//  Created by user on 3/24/17.
//  Copyright © 2017 alex. All rights reserved.
//

#import "NArt.h"
#import "Torch.h"
#include <Torch/Torch.h>


#define IMG_H 768
#define IMG_W 768


#define Log(str)         NSLog(@str)
#define Logw(str, ...)   NSLog(@str, __VA_ARGS__)

#define MEAN_R      103.939f
#define MEAN_G      116.779f
#define MEAN_B      123.68f
#define FloatTensor "torch.FloatTensor"

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__); assert(A); }

#define kInputImageChannels 3
#define kImageSize          (IMG_H * IMG_W * (kInputImageChannels + 1))
#define kInputTensorSize    (IMG_H * IMG_W * kInputImageChannels)

#define throwException(message) @throw [NSException                         \
                                        exceptionWithName: @"NArtException" \
                                        reason:   @message                  \
                                        userInfo: nil]

#define arraySize(array) (sizeof(array) / sizeof(array[0]))

@interface NArt()
    @property (nonatomic, strong) Torch *t;
@end

@implementation NArt

-(instancetype) init {
    if ( self = [super init] ) {
        self.t = [Torch new];

        [self.t initialize];
        [self.t runMain:    @"predictor" inFolder:   @""];
        
//        lua_State *L = [self.t getLuaState];
//        lua_pushcfunction(L, (lua_CFunction) lua_getTensorImage);
//        lua_setglobal(L, "getTensorImage");
//
//        lua_pushcfunction(L, (lua_CFunction) lua_saveImageTensor);
//        lua_setglobal(L, "saveImageTensor");
        
        [self.t loadFileWithName:   @"candy.t7"
                inResourceFolder:   @""
                andLoadMethodName:  @"initPredictor"];
        NSLog(@"torch inited.....");
        
        return self;
    }
    return nil;
}


//static void lua_getTensorImage(lua_State *L) {
////    int n = lua_gettop(L);
//    Log("lua: getTensorImage....");
//    THFloatTensor *testTensor = [nartSelf imageToTensor: [UIImage imageNamed: @"photo1.png"]];
//    luaT_pushudata(L, testTensor, FloatTensor);
//}
//
//static void lua_saveImageTensor(lua_State *L) {
//    THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, 1, FloatTensor);
//    assert(result != NULL);
//    Logw("received float tensor with size: %lu", (long) result->size);
//    UIImage *img = [nartSelf tensorToImage: result];
//    UIImageWriteToSavedPhotosAlbum(img, nil, nil, nil);
//}


-(void) styleImage: (UIImage *) image {
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        Log("starting converting ....");
        
        THFloatTensor *testTensor = [self imageToTensor: image];
        Log("ending  converting ....");

        lua_State *L = [self.t getLuaState];
        
        lua_getglobal(L, "styleImage");
        luaT_pushudata(L, testTensor, FloatTensor);
        
        Log("luaT pushudata ... ");

        if (lua_pcall(L, 1, 1, 0) != 0) {
            Logw("lua error str: %s ", lua_tostring(L, -1));
            throwException("Error calling lua function 'styleImage' ");
            return;
        }
        Log("begin tensor converting...");

        THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, -1, FloatTensor);
        assert(result != NULL);
        Logw("received float tensor with size: %lu", (long) result->size);
        
        lua_pop(L, 1);
        
        UIImage *outImage  = [self tensorToImage: result];
        UIImageWriteToSavedPhotosAlbum(outImage, nil, nil, nil);
    });
}


- (THFloatTensor *) imageToTensor: (UIImage *) image {

    NSLog(@"imageToTensor...");
    uint8_t *imageData = [self imageToArray: image];
    
    THFloatTensor *testTensor = THFloatTensor_newWithSize3d(3, IMG_W, IMG_H);
    
    NSLog(@"testTensorData created..., %i, %lu",
          testTensor->nDimension,
          (long) testTensor->size);
    
    for (int i = 0; i < kImageSize; i += 4) {
        @autoreleasepool {
            int j = i / 4;
            int yH = floorf(j / IMG_W);
            int xW = j - yH * IMG_W;
            THTensor_fastSet3d(testTensor,   0, xW, yH,    (imageData[i + 0] & 0xFF) - MEAN_R);
            THTensor_fastSet3d(testTensor,   1, xW, yH,    (imageData[i + 1] & 0xFF) - MEAN_G);
            THTensor_fastSet3d(testTensor,   2, xW, yH,    (imageData[i + 2] & 0xFF) - MEAN_B);
            
            //        THTensor_fastSet1d(testTensor,   0 * size + j,    (imageData[i + 0] & 0xFF) - MEAN_R); // red
            //        THTensor_fastSet1d(testTensor,   1 * size + j,    (imageData[i + 1] & 0xFF) - MEAN_G); // green
            //        THTensor_fastSet1d(testTensor,   2 * size + j,    (imageData[i + 2] & 0xFF) - MEAN_B); // blue
        }
    }
    NSLog(@"input tensor size %lu", (long) testTensor->size);
    free(imageData);
    return testTensor;
}



- (UIImage *) tensorToImage: (THFloatTensor *) imageTensor {
    NSLog(@"out tensor size: %lx", sizeof(imageTensor));
    
    uint8_t *imageBytes = new uint8_t[IMG_W * IMG_H  * 4];
    
    for (int i = 0; i < kImageSize; i += 4) {
        @autoreleasepool {
            int j = i / 4;
            int yH = floorf(j / IMG_W);
            int xW = j - yH * IMG_W;

            imageBytes[i + 0] = THTensor_fastGet3d(imageTensor,   0, xW, yH) + MEAN_R;
            imageBytes[i + 1] = THTensor_fastGet3d(imageTensor,   1, xW, yH) + MEAN_G;
            imageBytes[i + 2] = THTensor_fastGet3d(imageTensor,   2, xW, yH) + MEAN_B;
            
            //        imageBytes[i + 0] = THTensor_fastGet1d(imageTensor, 0 * size + j) + MEAN_R;
            //        imageBytes[i + 1] = THTensor_fastGet1d(imageTensor, 1 * size + j) + MEAN_G;
            //        imageBytes[i + 2] = THTensor_fastGet1d(imageTensor, 2 * size + j) + MEAN_B;
        }
    }
    
    free(imageTensor);
    UIImage *outImage = [self arrayToImage: imageBytes];
    return outImage;
}


- (uint8_t *) imageToArray: (UIImage *) image {
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    uint8_t *imageData = new uint8_t[kImageSize];
    CGContextRef contextRef = CGBitmapContextCreate(imageData,
                                                    IMG_W,
                                                    IMG_H,
                                                    8,
                                                    IMG_W * (kInputImageChannels + 1),
                                                    colorSpace,
                                                    kCGImageAlphaNoneSkipLast | kCGBitmapByteOrderDefault);
    CGContextDrawImage(contextRef, CGRectMake(0, 0, IMG_W, IMG_H), image.CGImage);
    CGContextRelease(contextRef);
    CGColorSpaceRelease(colorSpace);
    return  imageData;
}

-(UIImage *) arrayToImage: (uint8_t *) imageBytes {
    NSData *imageData = [NSData dataWithBytes: imageBytes  length: kImageSize * sizeof(uint8_t)];
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef) imageData);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    free(imageBytes);
    
    CGImageRef imageRef = CGImageCreate(IMG_W,
                                        IMG_H,
                                        8,
                                        8       * (kInputImageChannels + 1),
                                        IMG_W   * (kInputImageChannels + 1),
                                        colorSpace,
                                        kCGImageAlphaNone | kCGBitmapByteOrderDefault,
                                        provider,
                                        NULL,
                                        false,
                                        kCGRenderingIntentDefault);
    UIImage *outputImage = [UIImage imageWithCGImage: imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    
    return outputImage;
}

@end