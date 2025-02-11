using Newtonsoft.Json;
using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace idcard_C_
{
    internal class AES
    {
        public static string EncryptAES(string plainText)
        {
            string relativePath = "Key.json";
            string fullPath = Path.GetFullPath(relativePath);
            string json = File.ReadAllText(fullPath);
            
            dynamic jsonData = JsonConvert.DeserializeObject(json);

            // 암호화 키를 32바이트로 맞추기 (32바이트 키 사용)
            string encryptionKey = jsonData.aes;// 이 키는 반드시 32바이트여야 합니다.
            encryptionKey = encryptionKey.Substring(0, 32);  // 만약 길이가 32보다 크면 잘라내기

            using (Aes aesAlg = Aes.Create())
            {
                aesAlg.Key = Encoding.UTF8.GetBytes(encryptionKey);  // 암호화 키 설정 (16, 24, 32 바이트)
                aesAlg.IV = new byte[16];  // IV는 16 바이트로 설정 (기본 제로 초기화)

                ICryptoTransform encryptor = aesAlg.CreateEncryptor(aesAlg.Key, aesAlg.IV);

                using (MemoryStream msEncrypt = new MemoryStream())
                {
                    using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write))
                    {
                        using (StreamWriter swEncrypt = new StreamWriter(csEncrypt))
                        {
                            swEncrypt.Write(plainText);  // 텍스트 암호화
                        }
                    }
                    return Convert.ToBase64String(msEncrypt.ToArray());  // 암호화된 데이터를 Base64로 인코딩하여 리턴
                }
            }
        }

        public static string ProcessResidentNumber(string residentNumber)
        {
            // 공백 제거
            residentNumber = residentNumber.Trim();

            // 하이픈이 하나만 있는지 확인
            if (residentNumber.Count(c => c == '-') != 1)
            {
                throw new Exception("잘못된 주민번호 형식입니다. 하이픈이 정확히 하나여야 합니다.");
            }

            // 주민번호를 - 기준으로 분리합니다.
            var parts = residentNumber.Split('-');
            if (parts.Length != 2)
            {
                throw new Exception("잘못된 주민번호 형식입니다.");
            }

            // 앞 6자리 (변경하지 않음)
            string frontPart = parts[0];

            // 뒷 7자리 (암호화)
            string backPart = parts[1];

            // 뒷자리 첫 번째 자리는 그대로 두고 나머지 자리는 암호화 처리
            string backFirstChar = backPart[0].ToString();  // 첫 번째 자리
            string backPartToEncrypt = backPart.Substring(1);  // 첫 번째 자리를 제외한 나머지 부분

            // 나머지 뒷자리를 암호화
            string encryptedBackPart = EncryptAES(backPartToEncrypt);

            // 암호화된 주민번호 구성
            return frontPart + "-" + backFirstChar + encryptedBackPart;  // 앞 6자리 + 첫 번째 자리 + 암호화된 나머지 뒷자리
        }
    }
}
