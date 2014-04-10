package com.rsaspi.views;

import java.util.ArrayList;
import java.util.List;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;

import com.parse.FindCallback;
import com.parse.GetDataCallback;
import com.parse.ParseAnalytics;
import com.parse.ParseException;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.PushService;
import com.rsaspi.R;
import com.rsaspi.controllers.ActivityAdapter;
import com.rsaspi.models.RoomActivity;

/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

public class MainActivity extends Activity implements OnItemClickListener {

	private ActivityAdapter adapter;
	private ListView listView;

	/** Called when the activity is first created. */
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		ParseAnalytics.trackAppOpened(getIntent());
		ParseObject.registerSubclass(RoomActivity.class);
		PushService.setDefaultPushCallback(this, MainActivity.class);
		adapter = new ActivityAdapter(this, new ArrayList<RoomActivity>());
		listView = (ListView) findViewById(R.id.activity_list);
		listView.setAdapter(adapter);
		listView.setOnItemClickListener(this);

		updateData();
	}


	public void updateData() {
		ParseQuery<RoomActivity> query = ParseQuery
				.getQuery(RoomActivity.class);
		query.orderByDescending("enteredAt");
		query.findInBackground(new FindCallback<RoomActivity>() {
			@Override
			public void done(List<RoomActivity> activities, ParseException error) {
				if (error == null) {
					adapter.clear();
					for (int i = 0; i < activities.size(); i++) {
						adapter.add(activities.get(i));
					}
				} else {
					Log.d("rsaspi", "Error: " + error.getMessage());
				}
			}
		});
	}

	@Override
	public void onItemClick(AdapterView<?> parent, final View view,
			int position, long id) {
		RoomActivity activity = adapter.getItem(position);
		activity.photo().getDataInBackground(new GetDataCallback() {
			@SuppressLint("NewApi")
			@Override
			public void done(byte[] bytes, ParseException e) {
				Intent detail = new Intent(getApplicationContext(),
						DetailActivity.class);
				detail.putExtra("day", view.getTag(R.string.day).toString());
				detail.putExtra("date", view.getTag(R.string.date).toString());
				detail.putExtra("time", view.getTag(R.string.time).toString());
				detail.putExtra("bytes", bytes);
				startActivity(detail);
				overridePendingTransition(R.anim.right_slide_in,
						R.anim.right_slide_out);
			}
		});
	}
}
