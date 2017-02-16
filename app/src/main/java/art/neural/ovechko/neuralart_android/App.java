package art.neural.ovechko.neuralart_android;

import android.app.Application;

import art.neural.ovechko.neuralart.ArtUtil;
import art.neural.ovechko.neuralart.NeuralArt;

/**
 * Created by useruser on 2/16/17.
 */
public class App extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        NeuralArt.init(this);
    }

    @Override
    public void onLowMemory() {
        ArtUtil.log("onLowMemory....");

        super.onLowMemory();
    }
}
