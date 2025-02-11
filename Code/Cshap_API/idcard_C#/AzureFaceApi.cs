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
using Newtonsoft.Json;

namespace idcard_C_
{
    public static class AzureFaceApi
    {
        private static string relativePath = "Key.json";
        private static string fullPath = Path.GetFullPath(relativePath);
        private static string  json = File.ReadAllText(fullPath);

        private static dynamic jsonData = JsonConvert.DeserializeObject(json);

        public static string AzureFaceApiKey = jsonData.face;
        public const string AzureFaceApiEndpoint = "https://facetrue.cognitiveservices.azure.com/face/v1.0/detect";
        private static readonly HttpClient client = new HttpClient(); // 재사용 가능한 HttpClient

        static AzureFaceApi()
        {
            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", AzureFaceApiKey);
        }

        public static async Task<byte[]> DetectAndSaveFace(byte[] imageBytes)
        {
            var content = new ByteArrayContent(imageBytes)
            {
                Headers = { ContentType = new System.Net.Http.Headers.MediaTypeHeaderValue("application/octet-stream") }
            };

            var response = await client.PostAsync(AzureFaceApiEndpoint, content);
            if (!response.IsSuccessStatusCode)
                throw new Exception($"Azure Face API 요청 실패: {response.ReasonPhrase}");

            var jsonResponse = await response.Content.ReadAsStringAsync();
            Console.WriteLine("Azure Face API 응답 데이터: " + jsonResponse);

            var faces = JsonConvert.DeserializeObject<List<dynamic>>(jsonResponse);
            if (faces.Count == 0)
                throw new Exception("얼굴이 감지되지 않았습니다.");

            var faceRectangle = faces[0]?.faceRectangle;
            if (faceRectangle == null)
                throw new Exception("얼굴 데이터가 유효하지 않습니다.");

            Console.WriteLine($"얼굴 좌표: Left={faceRectangle.left}, Top={faceRectangle.top}, Width={faceRectangle.width}, Height={faceRectangle.height}");

            var originalImage = Cv2.ImDecode(imageBytes, ImreadModes.Color);
            if (originalImage.Empty())
                throw new Exception("이미지 디코딩에 실패했습니다.");

            var rect = new OpenCvSharp.Rect(
                Math.Max(0, (int)faceRectangle.left),
                Math.Max(0, (int)faceRectangle.top),
                Math.Min((int)faceRectangle.width, originalImage.Width - (int)faceRectangle.left),
                Math.Min((int)faceRectangle.height, originalImage.Height - (int)faceRectangle.top)
            );

            var faceImage = new Mat(originalImage, rect);
            Console.WriteLine($"잘라낸 이미지 크기: Width={faceImage.Width}, Height={faceImage.Height}");

            // 얼굴 이미지 크기 확대
            int newWidth = 120;  // 원하는 크기 (예: 640)
            int newHeight = 120; // 원하는 크기 (예: 480)
            Cv2.Resize(faceImage, faceImage, new OpenCvSharp.Size(newWidth, newHeight));  // 이미지 크기 변경
            Console.WriteLine($"변경된 이미지 크기: Width={faceImage.Width}, Height={faceImage.Height}");

            byte[] faceImageBytes;
            using (var ms = new MemoryStream())
            {
                Cv2.ImEncode(".jpg", faceImage, out var buffer);
                ms.Write(buffer, 0, buffer.Length);
                faceImageBytes = ms.ToArray();
            }

            return faceImageBytes;
        }
    }
}
