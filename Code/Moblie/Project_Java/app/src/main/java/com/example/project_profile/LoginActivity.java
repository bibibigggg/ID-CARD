package com.example.project_profile;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import org.json.JSONObject;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.URL;
import java.security.KeyStore;
import java.security.cert.CertificateFactory;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManagerFactory;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.os.Build;
import androidx.core.app.NotificationCompat;

public class LoginActivity extends AppCompatActivity {

    private EditText editText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        // 알림 권한 요청 (Android 13 이상)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.POST_NOTIFICATIONS}, 1);
            }
        }
        // 알림 채널 생성
        createNotificationChannel();

        // EditText 초기화
        editText = findViewById(R.id.editTextText);

        // 버튼 초기화 및 클릭 리스너 설정
        Button btn0 = findViewById(R.id.btn_0);
        Button btn1 = findViewById(R.id.btn_1);
        Button btn2 = findViewById(R.id.btn_2);
        Button btn3 = findViewById(R.id.btn_3);
        Button btn4 = findViewById(R.id.btn_4);
        Button btn5 = findViewById(R.id.btn_5);
        Button btn6 = findViewById(R.id.btn_6);
        Button btn7 = findViewById(R.id.btn_7);
        Button btn8 = findViewById(R.id.btn_8);
        Button btn9 = findViewById(R.id.btn_9);
        Button btnCheck = findViewById(R.id.btn_check);
        Button btnBack = findViewById(R.id.btn_back);

        // 숫자 버튼 클릭 리스너 설정
        View.OnClickListener numberClickListener = new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Button button = (Button) v;
                String currentText = editText.getText().toString();

                // 4자리 이상 입력되지 않도록 제한
                if (currentText.length() < 4) {
                    String buttonText = button.getText().toString();
                    editText.setText(currentText + buttonText); // 버튼에 해당하는 숫자를 EditText에 추가
                } else {
                    // 4자리 이상이면 입력을 막고, 사용자에게 알림
                    Toast.makeText(LoginActivity.this, "4자리 숫자만 입력 가능합니다.", Toast.LENGTH_SHORT).show();
                }
            }
        };

        // 각 버튼에 클릭 리스너 연결
        btn0.setOnClickListener(numberClickListener);
        btn1.setOnClickListener(numberClickListener);
        btn2.setOnClickListener(numberClickListener);
        btn3.setOnClickListener(numberClickListener);
        btn4.setOnClickListener(numberClickListener);
        btn5.setOnClickListener(numberClickListener);
        btn6.setOnClickListener(numberClickListener);
        btn7.setOnClickListener(numberClickListener);
        btn8.setOnClickListener(numberClickListener);
        btn9.setOnClickListener(numberClickListener);

        // '뒤로' 버튼 클릭 리스너 (EditText 내용 지우기)
        btnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String currentText = editText.getText().toString();
                if (currentText.length() > 0) {
                    // 마지막 문자 지우기
                    editText.setText(currentText.substring(0, currentText.length() - 1));
                }
            }
        });

        // '확인' 버튼 클릭 리스너 (인증번호 확인 후 화면 전환)
        btnCheck.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String inputText = editText.getText().toString();

                // 서버로 인증번호 전송 후 비교
                verifyPassword(inputText);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        // 최신 비밀번호 정보를 가져오는 메서드를 onResume에서 호출
        fetchLatestPassnumber();
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

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = getSystemService(NotificationManager.class);

            // 이미 채널이 존재하면 새로 만들지 않음
            if (notificationManager.getNotificationChannel("default_channel") == null) {
                CharSequence name = "Default Channel";
                String description = "This is the default notification channel";
                int importance = NotificationManager.IMPORTANCE_DEFAULT;
                NotificationChannel channel = new NotificationChannel("default_channel", name, importance);
                channel.setDescription(description);
                notificationManager.createNotificationChannel(channel);
            }
        }
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

    private void verifyPassword(final String inputPassword) {
        new Thread(() -> {
            try {
                Log.d("LoginActivity", "Password verification started");

                // 서버 URL 및 연결 설정
                URL url = new URL("https://192.168.0.220:5001/verify_password");
                HttpsURLConnection connection = (HttpsURLConnection) url.openConnection();
                connection.setSSLSocketFactory(getSslContext().getSocketFactory());
                connection.setRequestMethod("POST");
                connection.setDoOutput(true);
                connection.setRequestProperty("Content-Type", "application/json");

                // 호스트 이름 검증 우회
                HttpsURLConnection.setDefaultHostnameVerifier((hostname, session) -> true);

                // JSON 데이터 전송
                JSONObject jsonParam = new JSONObject();
                jsonParam.put("password", inputPassword);
                OutputStream os = connection.getOutputStream();
                os.write(jsonParam.toString().getBytes());
                os.flush();

                // 응답 처리
                int responseCode = connection.getResponseCode();
                Log.d("VerifyPassword", "Response Code: " + responseCode);

                if (responseCode == HttpsURLConnection.HTTP_OK) {
                    InputStream inputStream = connection.getInputStream();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
                    StringBuilder response = new StringBuilder();
                    String line;
                    while ((line = reader.readLine()) != null) {
                        response.append(line);
                    }
                    reader.close();

                    // 서버 응답 처리
                    JSONObject responseJson = new JSONObject(response.toString());
                    String status = responseJson.optString("status");
                    Log.d("VerifyPassword", "Server status: " + status);

                    if ("success".equals(status)) {
                        // 인증 성공 시 사용자 이름을 받아 ProfileActivity로 전달
                        String name = responseJson.optString("name");
                        String qrCodeData = responseJson.optString("qr_code");

                        // QR 코드 데이터가 존재한다면 전달
                        if (qrCodeData != null && !qrCodeData.isEmpty()) {
                            Log.d("QR_Code_Data", "Received QR Code Data: " + qrCodeData);

                            // 이름과 QR 코드 데이터를 ProfileActivity로 전달
                            Intent intent = new Intent(LoginActivity.this, ProfileActivity.class);
                            intent.putExtra("userName", name); // 이름을 전달
                            intent.putExtra("qr_code_data", qrCodeData); // QR 코드 데이터를 전달
                            startActivity(intent);
                            finish();
                        }
                    }
                } else {
                    Log.e("VerifyPassword", "Server returned error: " + responseCode);
                    runOnUiThread(() -> Toast.makeText(LoginActivity.this, "비밀번호가 잘못되었습니다.", Toast.LENGTH_SHORT).show());
                }

            } catch (Exception e) {
                e.printStackTrace();
                Log.e("VerifyPassword", "Error occurred: " + e.getMessage());
            }
        }).start();
    }
}
