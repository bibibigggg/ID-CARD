package com.example.project_profile;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import org.json.JSONObject;

public class ProfileActivity extends AppCompatActivity {

    private ImageView qrCodeImageView;
    private TextView tvName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        tvName = findViewById(R.id.tv_Name);

        // 이름을 Intent로 전달받음
        Intent intent = getIntent();
        String name = intent.getStringExtra("userName");

        // 이름을 TextView에 표시
        if (name != null) {
            tvName.setText(name);
        }
        // QR 코드 Base64 데이터 받기
        String jsonData = getIntent().getStringExtra("qr_code_data");

        if (jsonData != null) {
            try {
                Log.d("ProfileActivity", "QR Code JSON Data: " + jsonData); // JSON 데이터 확인용

                // JSON에서 Base64 문자열만 추출
                String qrCodeBase64 = extractBase64StringFromJson(jsonData);


                if (qrCodeBase64 != null) {
                    // Base64 문자열을 Bitmap으로 변환
                    byte[] decodedString = Base64.decode(qrCodeBase64, Base64.DEFAULT);
                    Bitmap decodedBitmap = BitmapFactory.decodeByteArray(decodedString, 0, decodedString.length);

                    // 이미지 뷰에 QR 코드 이미지 설정
                    ImageView imageView = findViewById(R.id.qrImage);
                    imageView.setImageBitmap(decodedBitmap);
                } else {
                    // Base64 문자열을 추출할 수 없는 경우
                    Log.e("ProfileActivity", "Failed to extract Base64 string");
                    Toast.makeText(this, "QR 코드 데이터를 추출할 수 없습니다.", Toast.LENGTH_SHORT).show();
                }



            } catch (IllegalArgumentException e) {
                // Base64 디코딩 오류 발생 시
                Log.e("ProfileActivity", "Base64 decoding failed", e);
                Toast.makeText(this, "QR 코드 디코딩 오류", Toast.LENGTH_SHORT).show();
            } catch (Exception e) {
                // 그 외 예외 처리
                Log.e("ProfileActivity", "Error loading QR code", e);
                Toast.makeText(this, "QR 코드 로딩 중 오류 발생", Toast.LENGTH_SHORT).show();
            }
        } else {
            // QR 코드 JSON 데이터가 null인 경우
            Log.e("ProfileActivity", "QR Code JSON data is null");
            Toast.makeText(this, "QR 코드 데이터를 받을 수 없습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    // JSON 데이터에서 Base64 문자열을 추출하는 메서드
    private String extractBase64StringFromJson(String jsonData) {
        String base64String = null;

        try {
            // JSON 문자열 파싱
            JSONObject jsonObject = new JSONObject(jsonData);

            // "qr_code" 키의 값이 없을 수도 있으므로, 기본적으로 "qr_code"를 찾고, 없으면 다른 키로도 검색
            if (jsonObject.has("qr_code")) {
                base64String = jsonObject.optString("qr_code");
            }

            if (base64String == null && jsonObject.has("qrImageBase64")) {
                base64String = jsonObject.optString("qrImageBase64");
            }

            // Base64 문자열에서 불필요한 공백 및 줄 바꿈을 제거
            if (base64String != null) {
                base64String = base64String.replace("\n", "").replace("\r", "").trim();

                // 패딩 문자 '='를 올바르게 추가하기 위해 문자열 길이를 확인하고, 짝수 길이가 아니면 패딩을 추가
                int mod4 = base64String.length() % 4;
                if (mod4 > 0) {
                    base64String = base64String + "=".repeat(4 - mod4);
                }
            }

        } catch (Exception e) {
            Log.e("ProfileActivity", "Error extracting Base64 string from JSON", e);
        }

        return base64String;
    }
}
