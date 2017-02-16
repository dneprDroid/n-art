package art.neural.ovechko.neuralart;


import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;

import java.nio.ByteBuffer;

public class TorchPredictor {

    public static final int IMG_H = 768;
    public static final int IMG_W = 768;
    private static TorchPredictor shared;

    private TorchPredictor() {
    }

    public static synchronized TorchPredictor with(Context context) {
        if (shared == null) {
            shared = new TorchPredictor();
            shared.initTorch(context);
        }
        return shared;
    }

    public static void free() {

    }

    protected void initTorch(Context context) {


        /*System.loadLibrary("gnustl_shared");
        //   System.loadLibrary("cublas");
        //System.loadLibrary("blas");*/

//        System.loadLibrary("loadcaffe");

        System.loadLibrary("luajit");
        System.loadLibrary("luaT");

        System.loadLibrary("TH");
        System.loadLibrary("torch");
        System.loadLibrary("THNN");
        //System.loadLibrary("THCUNN");

        //System.loadLibrary("image");

        System.loadLibrary("neuralart");

        String dir = context.getApplicationInfo().nativeLibraryDir;
        AssetManager manager = context.getAssets();
        nativeInitTorchPredictor(manager, dir);
    }

    public void styleImage(Bitmap bitmap) {
        ArtUtil.log("begin styling....");
        byte[] bytes = processBitmap(bitmap);
        nativeStyleImage(bytes);
    }

    public static byte[] processBitmap(Bitmap bitmap) {
        Bitmap scaled = Bitmap.createScaledBitmap(bitmap, IMG_W, IMG_H, false);
        ArtUtil.saveBitmap(scaled);
        ByteBuffer byteBuffer = ByteBuffer.allocate(scaled.getByteCount());
        scaled.copyPixelsToBuffer(byteBuffer);
        return byteBuffer.array();
    }

    native private void nativeInitTorchPredictor(AssetManager manager, String libLocation);

    native protected void nativeStyleImage(byte[] imageBytes);
}
