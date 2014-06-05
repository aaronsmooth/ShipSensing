package com.rsaspi;

import android.app.Application;

import com.parse.Parse;

/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

public class RsasApplication extends Application {

	@Override
	public void onCreate() {
		super.onCreate();
		Parse.initialize(this, "YwcaugA0e48pvA2Rsmj7yIT9GbOHCPitW3LDPnlq", "UMwUQlSXT7w8b9yX3KiNfSHr7JbHw8E1CFKB0LZ9");
	}
}
