package art.neural.ovechko.neuralart;

import android.graphics.Bitmap;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.util.Random;


public final class ArtUtil {
    //Debug
    public static void saveBitmap(final Bitmap bitmap) {
        File myDir = new File("/sdcard/saved_images");
        myDir.mkdirs();
        Random generator = new Random();
        int n = 10000;
        n = generator.nextInt(n);
        String fname = String.format("Image-%s.jpg", n);
        File file = new File(myDir, fname);
        if (file.exists()) file.delete();
        try {
            FileOutputStream out = new FileOutputStream(file);
            bitmap.compress(Bitmap.CompressFormat.JPEG, 90, out);
            out.flush();
            out.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void log(String s) {
        Log.d("NeuralArt", s);
    }
}
