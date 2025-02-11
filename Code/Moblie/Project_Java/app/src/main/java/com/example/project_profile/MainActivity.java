//package com.example.project_profile;
//
//import android.content.Intent;
//import android.os.Bundle;
//import android.os.Handler;
//import androidx.appcompat.app.AppCompatActivity;
//
//public class MainActivity extends AppCompatActivity {
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//
//        // 2초 후 LoginActivity로 이동
//        new Handler().postDelayed(() -> {
//            Intent intent = new Intent(MainActivity.this, LoginActivity.class);
//            startActivity(intent);
//            finish(); // MainActivity를 종료하여 뒤로 가기 버튼을 눌렀을 때 다시 돌아오지 않도록 설정
//        }, 2000); // 2000ms = 2초
//    }
//}

package com.example.project_profile;

import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.security.KeyStore;
import java.security.cert.CertificateFactory;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManagerFactory;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 2초 후 LoginActivity로 이동
        new Handler().postDelayed(() -> {
            // 인증번호를 가져오는 메서드를 호출
            fetchLatestPassnumber();

            // LoginActivity로 이동
            Intent intent = new Intent(MainActivity.this, LoginActivity.class);
            startActivity(intent);
            finish(); // MainActivity를 종료하여 뒤로 가기 버튼을 눌렀을 때 다시 돌아오지 않도록 설정
        }, 2000); // 2000ms = 2초
    }

    private void fetchLatestPassnumber() {
        new Thread(() -> {
            try {
                // 서버 URL 및 연결 설정 (올바른 엔드포인트로 수정)
                URL url = new URL("https://192.168.0.220:5001/latest_passnumber");
                HttpsURLConnection connection = (HttpsURLConnection) url.openConnection();
                connection.setRequestMethod("GET");
                connection.setSSLSocketFactory(getSslContext().getSocketFactory());
                connection.setDoInput(true);

                // 호스트 이름 검증 우회
                HttpsURLConnection.setDefaultHostnameVerifier((hostname, session) -> true);

                // 응답 코드 확인
                int responseCode = connection.getResponseCode();
                if (responseCode == HttpsURLConnection.HTTP_OK) {
                    InputStream inputStream = connection.getInputStream();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
                    StringBuilder response = new StringBuilder();
                    String line;
                    while ((line = reader.readLine()) != null) {
                        response.append(line);
                    }
                    reader.close();

                    // JSON 응답 처리
                    JSONObject responseJson = new JSONObject(response.toString());
                    String latestPassnumber = responseJson.optString("passnumber");  // passnumber로 수정

                    // 푸시 알림으로 passnumber 띄우기
                    runOnUiThread(() -> showPushNotification(latestPassnumber));
                } else {
                    Log.e("FetchPassword", "서버에서 오류가 발생했습니다: " + responseCode);
                }
            } catch (Exception e) {
                e.printStackTrace();
                Log.e("FetchPassword", "오류 발생: " + e.getMessage());
            }
        }).start();
    }

    private void showPushNotification(String latestPassword) {
        Log.d("Notification", "Sending notification with password: " + latestPassword);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, "default_channel")
                .setSmallIcon(R.drawable.logo) // 아이콘 설정
                .setContentTitle("최신 비밀번호")
                .setContentText("최신 비밀번호: " + latestPassword)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setAutoCancel(true);

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(1, builder.build());

        Log.d("Notification", "Notification sent.");
    }

    private SSLContext getSslContext() throws Exception {
        // 인증서 로드
        InputStream certInputStream = getResources().openRawResource(R.raw.server); // 서버 인증서 경로
        CertificateFactory cf = CertificateFactory.getInstance("X.509");
        java.security.cert.Certificate ca = cf.generateCertificate(certInputStream);
        certInputStream.close();

        // 키스토어 설정
        KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
        keyStore.load(null, null);
        keyStore.setCertificateEntry("ca", ca);

        // TrustManagerFactory 초기화
        TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        tmf.init(keyStore);

        // SSLContext 설정
        SSLContext sslContext = SSLContext.getInstance("TLS");
        sslContext.init(null, tmf.getTrustManagers(), new java.security.SecureRandom());
        return sslContext;
    }
}
