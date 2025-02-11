using IdCardRecognition;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using idcard_C_;

namespace idcard_C_
{
    public static class QrCodeGenerator
    {
        public static string GenerateQrCodeJson(string data)
        {
            if (string.IsNullOrEmpty(data))
                throw new Exception("QR 코드 데이터가 비어 있습니다.");

            // 데이터 분리
            string[] arr = data.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            string[] arr2 = new string[3];

            // 데이터에 따른 처리
            if (arr[0] == "주민등록증")
            {
                arr2[0] = arr[2];  // 주민등록번호
            }
            else
            {
                arr2[0] = arr[4];  // 주민등록번호
            }

            string aes = AES.ProcessResidentNumber(arr2[0]);

            // QR 코드 데이터로 사용할 내용 생성
            string qrData = $"{aes}"; // 필요한 데이터만 합쳐서 QR 코드에 사용할 문자열 생성

            // QR 코드 생성
            var barcodeWriter = new ZXing.BarcodeWriterPixelData
            {
                Format = ZXing.BarcodeFormat.QR_CODE,
                Options = new ZXing.QrCode.QrCodeEncodingOptions
                {
                    Width = 150,
                    Height = 150,
                    Margin = 1,
                    CharacterSet = "UTF-8" // UTF-8로 한글 지원
                }
            };

            var pixelData = barcodeWriter.Write(qrData); // QR 코드 이미지 데이터 생성

            // QR 코드 이미지를 저장할 경로 설정
            string qrImagePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "QRCode.png");

            // PixelData를 Bitmap으로 변환하여 이미지로 저장
            using (var bitmap = new Bitmap(pixelData.Width, pixelData.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb))
            {
                var bitmapData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), System.Drawing.Imaging.ImageLockMode.WriteOnly, bitmap.PixelFormat);
                System.Runtime.InteropServices.Marshal.Copy(pixelData.Pixels, 0, bitmapData.Scan0, pixelData.Pixels.Length);
                bitmap.UnlockBits(bitmapData);

                bitmap.Save(qrImagePath, System.Drawing.Imaging.ImageFormat.Png); // 파일로 저장
            }

            // QR 코드를 Base64로 변환
            string base64Data = ImageUtils.ConvertImageToBase64(qrImagePath);

            // JSON으로 반환
            return JsonConvert.SerializeObject(new { qr_code = base64Data });
        }
    }
}
