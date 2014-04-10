package com.rsaspi.models;

import java.util.Date;

import com.parse.ParseClassName;
import com.parse.ParseFile;
import com.parse.ParseObject;

/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

@ParseClassName("Activity")
public class RoomActivity extends ParseObject{
	public String id(){
		return getString("objectId");
	}
	public String photoUrl(){
		return getString("photoUrl");
	}
	
	public Date enteredAt(){
		return getDate("enteredAt");
	}
	
	public ParseFile photo(){
		return getParseFile("photo");
	}
}
