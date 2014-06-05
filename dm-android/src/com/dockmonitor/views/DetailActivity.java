package com.dockmonitor.views;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;

import com.dockmonitor.R;

/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

public class DetailActivity extends Activity {

	private TextView dayView;
	private TextView dateView;
	private TextView timeView;
	private ImageView imageView;

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.detail);
		Bundle extras = getIntent().getExtras();
		if (extras == null) {
			return;
		}
		// get data via the key
		String day = extras.getString("day").trim();
		String date = extras.getString("date").trim();
		String time = extras.getString("time").trim();
		byte[] bytes = extras.getByteArray("bytes");
		Bitmap bmp = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
		imageView = (ImageView) findViewById(R.id.photo);

		imageView.setImageBitmap(bmp);
		dayView = (TextView) findViewById(R.id.day);
		dayView.setText(day);
		dateView = (TextView) findViewById(R.id.date);
		dateView.setText(date);
		timeView = (TextView) findViewById(R.id.time);
		timeView.setText(time);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.action_back:
			onBackPressed();
			break;

		default:
			break;
		}
		return true;
	}
	
	@SuppressLint("NewApi")
	@Override
	public void onBackPressed() {
		this.finish();
		overridePendingTransition(R.anim.right_slide_in, R.anim.right_slide_out);
		return;
	}
}
