package edu.cornell.gdiac.rocketdemo;

import org.libsdl.app.*;
import android.os.*;

/**
 * Default application entry point
 */
public class RocketDemo extends SDLActivity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
	    // Make sure this is before calling super.onCreate
	    setTheme(R.style.CUGLTheme);
	    super.onCreate(savedInstanceState);
	}
	
}
