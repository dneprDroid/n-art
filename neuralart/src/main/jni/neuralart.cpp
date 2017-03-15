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
/*
bool isMainThread(JNIEnv *env) {
    jclass class1 = env->FindClass("art/neural/ovechko/neuralart/ArtUtil");


    jmethodID mid = env->GetStaticMethodID(class1, "isMainThread", "()B");

    jboolean isMainThread = env->CallStaticBooleanMethod(class1, mid);
    return (bool)(isMainThread == JNI_TRUE);
}


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
Java_art_neural_ovechko_neuralart_TorchPredictor_nativeStyleImage(JNIEnv *env, jobject thiz,
                                                       jbyteArray bitmapRGBData) {

    Log("nativeStyleImage....");
    luaFree();

    jbyte *inputData; // Initialize tensor to store java byte data from bitmap.
    inputData = (env)->GetByteArrayElements(bitmapRGBData,
                                            0); // Get pointer to java byte array region
    int result = -1;
    int size = IMG_W * IMG_H;
    THFloatTensor *testTensor = THFloatTensor_newWithSize1d(3 * size); //Initialize 1D tensor with size * 3 (R,G,B).
    //jfloat *testTensorData = THFloatTensor_data(testTensor);
    Log("testTensorData created...");

    for (int i = 0; i < size; i++) {
        THTensor_fastSet1d(testTensor,  size * 0 + i,    (inputData[i * 4 + 0] & 0xFF) - MEAN_R); // Red
        THTensor_fastSet1d(testTensor,  size * 1 + i,    (inputData[i * 4 + 1] & 0xFF) - MEAN_G); // Green
        THTensor_fastSet1d(testTensor,  size * 2 + i,    (inputData[i * 4 + 2] & 0xFF) - MEAN_B); // Blue
    }
    Log("image to tensor converted...");
    free(inputData);
    Log("cleaned jbyte array ");

    //Logw("checking thred type: %b", isMainThread(env));
    lua_getglobal(L, "styleImage");
    Log("luaT pushudata ...");

    luaT_pushudata(L, testTensor, FloatTensor);

    Log("luaT push data ended ...");

    if (lua_pcall(L, 1, 1, 0) != 0) {
        throwException(env, "Error calling lua function styleImage");
        Log(lua_tostring(L, -1));
    } else {
        Log("begin tensor converting...");

        THFloatTensor *result = (THFloatTensor *) luaT_toudata(L, 1, FloatTensor);
        assert(result != NULL);
        Log("received float tensor...");
        lua_pop(L, 1);
        jfloat *outTensorData = THFloatTensor_data(result);
        assert(outTensorData != NULL);
        Log("casting float tensor to jfloat is succeeded...");
    }
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
