package com.dockmonitor.models;

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
public class VesselActivity extends ParseObject{
	public String id(){
		return getString("objectId");
	}
	
	public Date dockedAt(){
		return getDate("dockedAt");
	}
	
	public String status(){
		return getString("status");
	}
	
	public ParseFile photo(){
		return getParseFile("image");
	}
}
