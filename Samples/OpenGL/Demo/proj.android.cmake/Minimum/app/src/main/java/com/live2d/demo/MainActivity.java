/**
 * Copyright(c) Live2D Inc. All rights reserved.
 * <p>
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

package com.live2d.demo;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;

public class MainActivity extends Activity {

    private GLSurfaceView _glSurfaceView;
    private GLRenderer _glRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        JniBridgeJava.SetActivityInstance(this);
        JniBridgeJava.SetContext(this);
        _glSurfaceView = new GLSurfaceView(this);
        _glSurfaceView.setEGLContextClientVersion(2);
        _glRenderer = new GLRenderer();
        _glSurfaceView.setRenderer(_glRenderer);
        _glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        setContentView(_glSurfaceView);

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT
                    ? View.SYSTEM_UI_FLAG_LOW_PROFILE
                    : View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
            );
        }
        else {
            getWindow().getInsetsController().hide(WindowInsets.Type.navigationBars()
                | WindowInsets.Type.statusBars());

            getWindow().getInsetsController().setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        JniBridgeJava.nativeOnStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        _glSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        _glSurfaceView.onPause();
        JniBridgeJava.nativeOnPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        JniBridgeJava.nativeOnStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        JniBridgeJava.nativeOnDestroy();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float pointX = event.getX();
        float pointY = event.getY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                JniBridgeJava.nativeOnTouchesBegan(pointX, pointY);
                break;
            case MotionEvent.ACTION_UP:
                JniBridgeJava.nativeOnTouchesEnded(pointX, pointY);
                break;
            case MotionEvent.ACTION_MOVE:
                JniBridgeJava.nativeOnTouchesMoved(pointX, pointY);
                break;
        }
        return super.onTouchEvent(event);
    }

}
