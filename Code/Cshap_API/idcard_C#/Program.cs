using Newtonsoft.Json;
using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MySql.Data.MySqlClient;
using OpenCvSharp;
using ZXing.SkiaSharp;
using System.Collections.Generic;
using System.Drawing;
using static System.Net.Mime.MediaTypeNames;
using idcard_C_;

namespace IdCardRecognition
{
    internal class Program
    {

        static async Task Main(string[] args)
        {
            // 데이터베이스 연결 문자열 설정
            string connectionString = "Server=3.36.108.189;Database=idcard;User ID=test;Password=1234;";

            try
            {
                Console.WriteLine("데이터베이스에서 이미지 가져오는 중...");
                var imageBlob = await DatabaseHandler.GetImageBlobFromDatabase(connectionString);
                Console.WriteLine("이미지 가져오기 완료!");

                // MemoryStream을 사용해 이미지 로드
                using (var memoryStream = new MemoryStream(imageBlob))
                {
                    Console.WriteLine("텍스트 추출 시작...");
                    var extractedText = await GoogleVisionApi.ExtractTextFromImage(memoryStream.ToArray());
                    Console.WriteLine("텍스트 추출 완료!");

                    Console.WriteLine("얼굴 인식 시작...");
                    var faceImageBytes = await AzureFaceApi.DetectAndSaveFace(memoryStream.ToArray());  // MemoryStream을 byte[]로 변환
                    Console.WriteLine("얼굴 인식 완료!");

                    Console.WriteLine("QR 코드 생성 시작...");
                    var qrCodeJson = QrCodeGenerator.GenerateQrCodeJson(extractedText);
                    Console.WriteLine("QR 코드 생성 완료!");

                    Console.WriteLine("데이터베이스 저장 시작...");
                    await DatabaseHandler.SaveToDatabase(extractedText, faceImageBytes, qrCodeJson);
                    Console.WriteLine("데이터베이스 저장 완료!");
                }

                Console.WriteLine("작업이 성공적으로 완료되었습니다.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"오류 발생: {ex.Message}");
            }
        }
    }
}
