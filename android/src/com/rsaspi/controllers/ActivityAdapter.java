package com.rsaspi.controllers;

import java.util.Calendar;
import java.util.Date;
import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.rsaspi.R;
import com.rsaspi.models.VesselActivity;

/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

public class ActivityAdapter extends ArrayAdapter<VesselActivity> {
	private Context context;
	private List<VesselActivity> activities;

	public ActivityAdapter(Context context, List<VesselActivity> activities) {
		super(context, R.layout.main, activities);
		this.context = context;
		this.activities = activities;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		if (convertView == null) {
			LayoutInflater inflater = LayoutInflater.from(context);
			convertView = inflater.inflate(R.layout.activity_row_item, null);
		}

		VesselActivity activity = activities.get(position);
		TextView descriptionView = (TextView) convertView
				.findViewById(R.id.activity_id);

		Date arrivalDate = activity.dockedAt();
		String enteredAt = arrivalDate.toLocaleString();
		System.out.println(enteredAt);
		String date = enteredAt.substring(0, enteredAt.indexOf("2014") + 4);
		String time = enteredAt.substring(enteredAt.indexOf("2014") + 4,
				enteredAt.length());
		String day = getStringDay(arrivalDate.getDay());

		convertView.setTag(R.string.day, day);
		convertView.setTag(R.string.date, date);
		convertView.setTag(R.string.time, time);

		descriptionView.setText(day + ", " + date + " @ " + time);
		return convertView;
	}

	private String getStringDay(int day) {
		String stringDay = null;
		switch (day + 1) {
		case Calendar.SUNDAY:
			stringDay = "Sunday";
			break;
		case Calendar.MONDAY:
			stringDay = "Monday";
			break;
		case Calendar.TUESDAY:
			stringDay = "Tuesday";
			break;
		case Calendar.WEDNESDAY:
			stringDay = "Wednesday";
			break;
		case Calendar.THURSDAY:
			stringDay = "Thursday";
			break;
		case Calendar.FRIDAY:
			stringDay = "Friday";
			break;
		case Calendar.SATURDAY:
			stringDay = "Saturday";
			break;
		default:
			break;
		}
		return stringDay;
	}
}
