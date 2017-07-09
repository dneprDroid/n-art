#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "torchandroid.h"
#include <assert.h>
#include <android/bitmap.h>
#include "include/luaT.h"


extern "C" {


///***** Predictor *****////////

#define IMG_H 768
#define IMG_W 768

#define MEAN_R 103.939f
#define MEAN_G 116.779f
#define MEAN_B 123.68f

#define kInputImageChannels 3
#define kImageSize          (IMG_H * IMG_W * (kInputImageChannels + 1))
#define kInputTensorSize    (IMG_H * IMG_W * kInputImageChannels)


#define FloatTensor "torch.FloatTensor"
#define arraySize(_array) (sizeof(_array) / sizeof(_array[0]))
#define Logw(string, x...)        __android_log_print(ANDROID_LOG_INFO, "NeuralArt", string, x)

#define Log(string)                 __android_log_print(ANDROID_LOG_INFO, "NeuralArt", string)
#define throwException(env, message)        env->ThrowNew(env->FindClass("art/neural/ovechko/neuralart/NArtException"), message)

#define KBYTES_CLEAN_UP 10000
#define LUAT_STACK_INDEX_FLOAT_TENSORS 4
#define GC_LUA_SIZE                lua_gc(L, LUA_GCCOUNT, LUAT_STACK_INDEX_FLOAT_TENSORS)
#define isTimeToCleanLuaGC()       GC_LUA_SIZE >= KBYTES_CLEAN_UP


static lua_State *L; // Lua state


void luaFree() {

      if (isTimeToCleanLuaGC())
      {
        Log("LUA -> Cleaning Up Garbage");
        lua_gc(L, LUA_GCCOLLECT, LUAT_STACK_INDEX_FLOAT_TENSORS);
      } else {
        Logw("Lua GC size: %d", GC_LUA_SIZE);
      }
}




THFloatTensor * arrayToTensor(jbyte *imageData) {
    THFloatTensor *testTensor = THFloatTensor_newWithSize3d(3, IMG_W, IMG_H);
//    NSLog(@"testTensorData created..., %i, %lu",
//          testTensor->nDimension,
//          (long) testTensor->size);

    for (int i = 0; i < kImageSize; i += 4) {
            int j = i / 4;
            int yH = floorf(j / IMG_W);
            int xW = j - yH * IMG_W;
            THTensor_fastSet3d(testTensor,   0, xW, yH,    (imageData[i + 0] & 0xFF) - MEAN_R);
            THTensor_fastSet3d(testTensor,   1, xW, yH,    (imageData[i + 1] & 0xFF) - MEAN_G);
            THTensor_fastSet3d(testTensor,   2, xW, yH,    (imageData[i + 2] & 0xFF) - MEAN_B);
    }
//    NSLog(@"input tensor size %lu", (long) testTensor->size);
//    free(imageData);
    return testTensor;
}

JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_TorchPredictor_nativeStyleImage(JNIEnv *env, jobject thiz,
                                                       jbyteArray bitmapRGBData) {
    Log("nativeStyleImage....");
    luaFree();

    jbyte *inputData; // Initialize tensor to store java byte data from bitmap.
    inputData = (env)->GetByteArrayElements(bitmapRGBData, 0); // Get pointer to java byte array region
    Log("starting converting ....");

    THFloatTensor *testTensor = arrayToTensor(inputData);
    free(inputData);
    Log("ending  converting ....");

//    lua_State *L = [self.t getLuaState];

    lua_getglobal(L, "styleImage");
    luaT_pushudata(L, testTensor, FloatTensor);

    Log("luaT pushudata ... ");

    if (lua_pcall(L, 1, 1, 0) != 0) {
        Logw("lua error str: %s ", lua_tostring(L, -1));
        throwException(env,"Error calling lua function 'styleImage' ");
        return;
    }
    Log("begin tensor converting...");

    THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, -1, FloatTensor);
    assert(result != NULL);
    Logw("received float tensor with size: %lu", (long) result->size);

    lua_pop(L, 1);
    env->ReleaseByteArrayElements(bitmapRGBData, inputData,
                                  0); // Destroy pointer to location in C. Only need java now
}


JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_TorchPredictor_nativeInitTorchPredictor(JNIEnv *env, jobject thiz,
                                                               jobject assetManager,
                                                               jstring _nativeLibraryDir) {
    //jint rs = env->GetJavaVM(&jvm);
    //assert (rs == JNI_OK);

    AAssetManager *manager = AAssetManager_fromJava(env, assetManager);
    assert(manager != NULL);
    const char *nativeLibraryDir = env->GetStringUTFChars(_nativeLibraryDir, 0);
    //lua_State *L = NULL;
    L = inittorch(manager, nativeLibraryDir); // create a lua_State

    char file[] = "predictor.lua";
    int ret;
    long size = android_asset_get_size(file);
    if (size != -1) {
        char *filebytes = android_asset_get_bytes(file);
        ret = luaL_dobuffer(L, filebytes, size, "predictor");
        if (ret == 1) {
            throwException(env, "Cannot find asset file predictor.lua");
            D("Torch Error doing resource: %s\n", file);
            D(lua_tostring(L, -1));
        } else {
            D("Torch script run successfully: ");
        }
    }
    lua_getglobal(L, "initPredictor"); // call lua function
    Log("inited predicior...");
}

JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_NeuralArt_nativeFree(JNIEnv *env, jobject thiz) {
    Log("Lua gc running....");
    luaFree();
    free(L);
}
}
