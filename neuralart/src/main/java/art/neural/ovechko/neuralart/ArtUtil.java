package art.neural.ovechko.neuralart;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Random;


public final class ArtUtil {
    //Debug
    public static void saveBitmap(final Bitmap bitmap) {
        File myDir = new File("/sdcard/saved_images");
        myDir.mkdirs();
        String fname = String.format("Image-%s.jpg", randomInt(1000));
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

    @Nullable
    public static byte[] readFileBytes(String path) {
        try {
            InputStream inputStream = new FileInputStream(path);
            byte[] b = new byte[1024];
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            int c;
            while ((c = inputStream.read(b)) != -1) {
                os.write(b, 0, c);
            }
            return os.toByteArray();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static boolean isServiceAvailable(Context context, Class<? extends Service> serviceClass) {
        final PackageManager packageManager = context.getPackageManager();
        final Intent intent = new Intent(context, serviceClass);
        List resolveInfo =
                packageManager.queryIntentServices(intent,
                        PackageManager.MATCH_DEFAULT_ONLY);
        if (resolveInfo.size() > 0) {
            return true;
        }
        return false;
    }

    public static int randomInt(int i) {
        return new Random().nextInt(i);
    }
}
