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
#define FloatTensor "torch.FloatTensor"
#define arraySize(_array) (sizeof(_array) / sizeof(_array[0]))
#define Log(string, args...)        __android_log_print(ANDROID_LOG_INFO, "NeuralArt", string, args)
#define Log(string)                 __android_log_print(ANDROID_LOG_INFO, "NeuralArt", string)
#define throwException(env, message)        env->ThrowNew(env->FindClass("java/lang/Exception"), message)

lua_State *L; // Lua state


int *styleImage(int *pixels, int w, int h) {
    float *imageTensor = new float[w * h * 3 * sizeof(float)];
    int img_lenght = w * h * 4;
    for (int i = 0; i < img_lenght; i += 4) {
        int j = i / 4;
        imageTensor[0 * IMG_W * IMG_H + j] =
                (float) (((int) (pixels[i + 0])) & 0xFF) - MEAN_R; // red
        imageTensor[1 * IMG_W * IMG_H + j] =
                (float) (((int) (pixels[i + 1])) & 0xFF) - MEAN_G; // green
        imageTensor[2 * IMG_W * IMG_H + j] =
                (float) (((int) (pixels[i + 2])) & 0xFF) - MEAN_B; // blue
    }
    free(pixels);

    //luaT_pushudata(L, testTensor, FloatTensor);
}
/*
//called from Lua
static void onImageStyled(lua_State *L) {
    JNIEnv *env;
    jint rs = jvm->AttachCurrentThread(&env, NULL);
    assert (rs == JNI_OK);

    THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, 1, "torch.FloatTensor");
    assert(result != NULL);

    //output tensor to java array
    jfloatArray jimageTensor;
    float *tensorArray = result->storage->data;
    jimageTensor = env->NewFloatArray(arraySize(tensorArray));
    env->SetFloatArrayRegion(jimageTensor, 0, 3, tensorArray);
    free(tensorArray);

    jclass clazz = env->FindClass("art/neural/ovechko/neuralart/NeuralArt");
    jmethodID onCompleted = env->GetStaticMethodID(clazz, "onImageStyled",
                                                   "()V");
    env->CallStaticVoidMethod(clazz, onCompleted, jimageTensor);
    env->DeleteLocalRef(clazz);
    //env->DeleteLocalRef(onCompleted);
}
*/

JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_NeuralArt_styleImage(JNIEnv *env, jobject thiz,
                                                       jbyteArray bitmapRGBData) {
    jbyte *inputData; // Initialize tensor to store java byte data from bitmap.
    inputData = (env)->GetByteArrayElements(bitmapRGBData,
                                            0);//Get pointer to java byte array region
    int result = -1;
    int size = IMG_W * IMG_H;
    THFloatTensor *testTensor = THFloatTensor_newWithSize1d(
            3 * size); //Initialize 1D tensor with size * 3 (R,G,B).
    jfloat *testTensorData = THFloatTensor_data(testTensor);
    Log("testTensorData created...");

    for (int i = 0; i < size; i++) {
        testTensorData[size * 0 + i] = (inputData[i * 4 + 0] & 0xFF) - MEAN_R; // Red
        testTensorData[size * 1 + i] = (inputData[i * 4 + 1] & 0xFF) - MEAN_G; // Green
        testTensorData[size * 2 + i] = (inputData[i * 4 + 2] & 0xFF) - MEAN_B; // Blue
    }
    Log("image to tensor converted...");
    lua_getglobal(L, "styleImage");
    Log("luaT pushudata ...");
    luaT_pushudata(L, testTensor, FloatTensor);
    //free(testTensor);

    if (lua_pcall(L, 1, 1, 0) != 0) {
        //Call function. Print error if call not successful
        throwException(env, "Error calling lua function styleImage");
        //Log("Error running function: %s", lua_tostring(L, -1));
    } else {
        THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, 1, FloatTensor);
        assert(result != NULL);
        Log("received float tensor...");
        lua_pop(L, 1);
        jfloat *outTensorData = THFloatTensor_data(result);
        assert(outTensorData != NULL);
        Log("casting float tensor to jfloat is succeeded...");
    }
    env->ReleaseByteArrayElements(bitmapRGBData, inputData,
                                  0); //Destroy pointer to location in C. Only need java now
}

JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_NeuralArt_initTorchPredictor(JNIEnv *env, jobject thiz,
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
}
}
