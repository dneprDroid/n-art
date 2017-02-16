package art.neural.ovechko.neuralart;

import android.content.Context;
import android.content.SyncAdapterType;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.nio.ByteBuffer;

/**
 * Created by useruser on 2/10/17.
 */
public class NeuralArt {
    private static final int IMG_H = 768;
    private static final int IMG_W = 768;
    private static NeuralArt shared;

    private NeuralArt() {
    }

    public static synchronized NeuralArt with(Context context) {
        if (shared == null) {
            shared = new NeuralArt();
            shared.initTorch(context);
        }
        return shared;
    }

    public static void init(final Context context) {
        ThreadManager.getInstance().execute(new Runnable() {
            @Override
            public void run() {
                with(context);
            }
        });
    }

    public static void free() {

    }

    private void initTorch(Context context) {


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

        ArtUtil.log("torchandroid loading... ");
        //System.loadLibrary("torchandroid");

        System.loadLibrary("image");


        System.loadLibrary("neuralart");

        String dir = context.getApplicationInfo().nativeLibraryDir;
        AssetManager manager = context.getAssets();
        initTorchPredictor(manager, dir);
    }

    public void styleImage(Bitmap bitmap) {
        Bitmap scaled = Bitmap.createScaledBitmap(bitmap, IMG_W, IMG_H, false);
        ArtUtil.saveBitmap(scaled);
        ByteBuffer byteBuffer = ByteBuffer.allocate(scaled.getByteCount());
        scaled.copyPixelsToBuffer(byteBuffer);
        ArtUtil.log("begin styling....");
        styleImage(byteBuffer.array());
    }

    native private void initTorchPredictor(AssetManager manager, String libLocation);

    native private void styleImage(byte[] imageBytes);
}
