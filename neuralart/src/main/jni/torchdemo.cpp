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

lua_State *L; // Lua state


int *styleImage(int *pixels, int w, int h) {

}


JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_NeuralArt_styleImage(JNIEnv *env, jobject thiz,
                                                       jbyteArray bitmapRGBData) {


}

JNIEXPORT void  JNICALL
Java_art_neural_ovechko_neuralart_NeuralArt_initTorchPredictor(JNIEnv *env, jobject thiz,
                                                               jobject assetManager,
                                                               jstring _nativeLibraryDir) {


}
}
