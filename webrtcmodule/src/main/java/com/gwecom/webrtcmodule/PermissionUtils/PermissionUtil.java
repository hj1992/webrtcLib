package com.gwecom.webrtcmodule.PermissionUtils;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.PermissionChecker;
import android.util.Log;

import java.util.ArrayList;



public class PermissionUtil {
    public static String TAG = "PermissionHelper";

    public static boolean verifyPermissions(int[] grantResults) {
        // Verify that each required permission has been granted, otherwise return false.
        for (int result : grantResults) {
            if (result != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }


    public static boolean checkSelfPermissions(Context context, String[] permissionName){


        boolean result = false;
        for (String permission : permissionName) {

           // if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {

                if (context.getApplicationInfo().targetSdkVersion>= Build.VERSION_CODES.M) {
                    // targetSdkVersion >= Android M,
                    result = (ActivityCompat.checkSelfPermission(context,permission) == PackageManager.PERMISSION_GRANTED);
                } else {
                    // targetSdkVersion < Android M
                    result = (PermissionChecker.checkSelfPermission(context, permission)== PermissionChecker.PERMISSION_GRANTED);
                }
                if (!result)
                {
                    return result;
                }
            }
        //}
        return result;
    }

    /**
     *判断 权限是否在AndroidManifest.xml声请
     */
    public static  boolean permissionExists(Context context, @NonNull String permissionName) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_PERMISSIONS);
            if (packageInfo.requestedPermissions != null) {
                for (String p : packageInfo.requestedPermissions) {
                    if (p.equals(permissionName)) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }


    /**
     * 得到所有没有获得权限
     */
    public static String[] getDeniedPermissons(Context context, String[] permissionName){
        ArrayList<String> needPermissons = new ArrayList<>();
        for (String permission: permissionName){
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                boolean result=false;
                if (context.getApplicationInfo().targetSdkVersion>= Build.VERSION_CODES.M) {
                    // targetSdkVersion >= Android M,
                    Log.i(TAG,"getDeniedPermissons targetSdkVersion>=M ");
                    result = (ActivityCompat.checkSelfPermission(context,permission) != PackageManager.PERMISSION_GRANTED);
                } else {
                    // targetSdkVersion < Android M
                    Log.i(TAG,"getDeniedPermissons targetSdkVersion<M ");
                    result = (PermissionChecker.checkSelfPermission(context, permission)!= PermissionChecker.PERMISSION_GRANTED);
                }

                if (result)
                {
                    needPermissons.add(permission);
                }
            }
//            if (ActivityCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
//                needPermissons.add(permission);
//            }
        }
        return needPermissons.toArray(new String[needPermissons.size()]);
    }

}
