using Newtonsoft.Json;
using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using idcard_C_;

namespace idcard_C_
{
    public static class GoogleVisionApi
    {
        private static string relativePath = "Key.json";
        private static string fullPath = Path.GetFullPath(relativePath);
        private static string json = File.ReadAllText(fullPath);

        private static dynamic jsonData = JsonConvert.DeserializeObject(json);

        public static string GoogleVisionApiKey = jsonData.google;
        private static readonly HttpClient client = new HttpClient(); // 재사용 가능한 HttpClient

        // byte[]로 이미지 데이터를 받는 메서드 (새로 추가)
        public static async Task<string> ExtractTextFromImage(byte[] imageBytes)
        {
            string apiUrl = $"https://vision.googleapis.com/v1/images:annotate?key={GoogleVisionApiKey}";

            var requestBody = new
            {
                requests = new[]
                {
                new
                {
                    image = new { content = Convert.ToBase64String(imageBytes) }, // 이미지 데이터를 Base64로 변환
                    features = new[] { new { type = "TEXT_DETECTION" } }
                }
            }
            };

            var response = await client.PostAsync(apiUrl, new StringContent(JsonConvert.SerializeObject(requestBody), Encoding.UTF8, "application/json"));
            if (!response.IsSuccessStatusCode)
            {
                throw new Exception($"Google Vision API 요청 실패: {response.ReasonPhrase}");
            }

            var jsonResponse = await response.Content.ReadAsStringAsync();
            var result = JsonConvert.DeserializeObject<dynamic>(jsonResponse);
            return result.responses[0]?.fullTextAnnotation?.text ?? throw new Exception("텍스트를 추출하지 못했습니다.");
        }
    }
}
