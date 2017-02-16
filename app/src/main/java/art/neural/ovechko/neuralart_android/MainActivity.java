package art.neural.ovechko.neuralart_android;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.FileNotFoundException;
import java.io.InputStream;

import art.neural.ovechko.neuralart.ArtUtil;
import art.neural.ovechko.neuralart.NeuralArt;

public class MainActivity extends AppCompatActivity {

    public static final int PICKER_CODE = 100;
    private Uri imageUri;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btnSelect).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
                photoPickerIntent.setType("image/*");
                startActivityForResult(photoPickerIntent, PICKER_CODE);
            }
        });


        findViewById(R.id.btnStyle).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    InputStream imageStream = getContentResolver().openInputStream(imageUri); // content://media/external/images/media/18306
                    Bitmap bitmap = BitmapFactory.decodeStream(imageStream);

                    NeuralArt.styleImage(MainActivity.this, bitmap);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
            }
        });
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        ArtUtil.log("onActivityResult....");
        super.onActivityResult(requestCode, resultCode, data);

        boolean resultOk = resultCode == RESULT_OK && requestCode == PICKER_CODE && data != null;
        if (resultOk) {
            imageUri = data.getData();
            Log.v("", "image uri: " + imageUri.toString());
        } else Log.e("", "result isn't ok....");
    }
}
