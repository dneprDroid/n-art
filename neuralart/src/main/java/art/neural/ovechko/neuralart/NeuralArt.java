package art.neural.ovechko.neuralart;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SyncAdapterType;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.IBinder;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Random;


public class NeuralArt {


    private static ArtService artService;
    private static ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            ArtService.MyBinder myBinder = (ArtService.MyBinder) service;
            artService = myBinder.getService();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    private NeuralArt() {
    }


    public static void init(final Context context) {
        ArtUtil.log("init....");
        Intent serviceIntent = new Intent(context, ArtService.class);
        startService(context, serviceIntent);
        context.bindService(serviceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    public static void styleImage(final Context context, Bitmap bitmap) {
        if (artService == null) {
            throw new NArtException("Missing this line in your Application class (in onCreate method): \n " +
                    "NeuralArt.init(context);");
        }
        ArtUtil.log("styling image.....");

        String fname = String.format("neural-art-%s.jpg", ArtUtil.randomInt(1000));
        File file = new File(context.getCacheDir(), fname);
        byte[] imageBytes = TorchPredictor.processBitmap(bitmap);

        try {
            FileOutputStream out = new FileOutputStream(file);
            out.write(imageBytes);
            out.flush();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        artService.styleImage(file.getAbsolutePath());
    }

    private static void startService(Context context, Intent serviceIntent) {
        if (!ArtUtil.isServiceAvailable(context, ArtService.class)) {
            String text =
                    "Missing declaring ArtService in manifest! \n"
                            + "<service \n"
                            + "android:name=\"art.neural.ovechko.neuralart.ArtService\" \n"
                            + "android:process=\":ArtService\" /> \n";
            throw new NArtException(text);
        }
        context.startService(serviceIntent);
    }
}
