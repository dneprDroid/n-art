package art.neural.ovechko.neuralart;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.text.TextUtils;


public class ArtService extends Service {
    public static final String IMAGE_DATA = "IMAGE_DATA";
    private IBinder mBinder = new MyBinder();


    @Override
    public void onCreate() {
        super.onCreate();
        runTask(new Runnable() {
            @Override
            public void run() {
                TorchPredictor.with(ArtService.this);
            }
        });


    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        ArtUtil.log("received new image");

        return START_STICKY;
    }

    public void styleImage(String path) {
        final byte[] imageData = ArtUtil.readFileBytes(path);
        if (imageData == null) {
            ArtUtil.log("Bitmap is empty....");
            return;
        }
        runTask(new Runnable() {
            @Override
            public void run() {
                TorchPredictor.with(ArtService.this).nativeStyleImage(imageData);
            }
        });
    }

    @Override
    public void onDestroy() {
        ArtUtil.log("service destroyed....");
        TorchPredictor.free();
        super.onDestroy();

    }

    @Override
    public IBinder onBind(Intent intent) {
        ArtUtil.log("in onBind");
        return mBinder;
    }

    @Override
    public void onRebind(Intent intent) {
        ArtUtil.log("in onRebind");
        super.onRebind(intent);
    }

    @Override
    public boolean onUnbind(Intent intent) {
        ArtUtil.log("in onUnbind");
        return true;
    }

    private void runTask(Runnable runnable) {
        ThreadManager.getInstance().execute(runnable);
    }

    public class MyBinder extends Binder {
        ArtService getService() {
            return ArtService.this;
        }
    }
}
