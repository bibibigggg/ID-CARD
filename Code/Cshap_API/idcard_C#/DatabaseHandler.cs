using Newtonsoft.Json;
using System;
using System.Threading.Tasks;
using System.IO;
using MySql.Data.MySqlClient;
using idcard_C_;
using Google.Type;

namespace idcard_C_
{
    public static class DatabaseHandler
    {
        private static string PH;

        public static async Task SaveToDatabase(string text, byte[] faceImage, string qrCodeJson)
        {
            string name, pnumber, address;
            int passnumber;

            var formattedTextArray = text.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries); // \n을 기준으로 텍스트를 나눠 배열에 담음
            if (formattedTextArray.Length < 5) // 주민등록증의 최소 줄수보다 작을때
                throw new Exception("텍스트 데이터가 유효하지 않습니다.");

            // 텍스트에서 값 추출
            if (formattedTextArray[0] == "주민등록증")
            {
                name = formattedTextArray[1]; // 이름
                pnumber = formattedTextArray[2]; // 주민등록번호
                if (formattedTextArray[4].Contains(",") || formattedTextArray[4].Contains(".")) // 주소가 한줄로 끝날때
                { // 날짜엔 "." OR ","가 붙음
                    address = formattedTextArray[3];
                }
                else
                {
                    address = formattedTextArray[3] + formattedTextArray[4];
                }
            }
            else
            {
                name = formattedTextArray[3];
                pnumber = formattedTextArray[4];
                if (formattedTextArray[5].Contains(",") || formattedTextArray[5].Contains(".") )
                {
                    address = formattedTextArray[5];
                }
                else
                {
                    address = formattedTextArray[5] + formattedTextArray[6];
                }
            }

            passnumber = new Random().Next(1000, 10000);

            pnumber = AES.ProcessResidentNumber(pnumber);

            //string faceJsonFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "faceData.json");
            //string faceJsonData = File.ReadAllText(faceJsonFilePath); // 파일에서 JSON 읽기

            string relativePath = "Key.json";
            string fullPath = Path.GetFullPath(relativePath);
            string json = File.ReadAllText(fullPath); 

            dynamic jsonData = JsonConvert.DeserializeObject(json);// database의 정보를 하드코딩을 막기위한 Json파일 사용

            // MySQL 연결 설정
            string connectionString = jsonData.data;

            using (var connection = new MySqlConnection(connectionString))
            {
                try
                {
                    await connection.OpenAsync();
                    Console.WriteLine("MySQL 연결 성공");

                    string query = "INSERT INTO idcard (name, pnumber, address, passnumber, qr, face, phnumber) VALUES (@name, @pnumber, @address, @passnumber, @qr, @face, @phnumber)";
                    using (var cmd = new MySqlCommand(query, connection))
                    {
                        cmd.Parameters.AddWithValue("@name", name);
                        cmd.Parameters.AddWithValue("@pnumber", pnumber);
                        cmd.Parameters.AddWithValue("@address", address);
                        cmd.Parameters.AddWithValue("@passnumber", passnumber); // 랜덤 4자리(어플 비밀번호)
                        cmd.Parameters.AddWithValue("@qr", qrCodeJson); // QR코드 Json형태로 저장
                        cmd.Parameters.AddWithValue("@face", faceImage);  //BLOB형태로 저장
                        cmd.Parameters.AddWithValue("@phnumber", PH);

                        int rowsAffected = await cmd.ExecuteNonQueryAsync();
                        if (rowsAffected > 0)
                        {
                            Console.WriteLine("데이터베이스에 데이터가 성공적으로 저장되었습니다.");
                        }
                        else
                        {
                            Console.WriteLine("데이터베이스에 데이터를 저장하는 데 실패했습니다.");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"MySQL 연결 실패: {ex.Message}");
                }
            }
        }
        public static async Task<byte[]> GetImageBlobFromDatabase(string connectionString)
        {
            using (var connection = new MySqlConnection(connectionString))
            {
                try
                {
                    await connection.OpenAsync();

                    // 테이블에 하나의 데이터만 있으므로 LIMIT 1 사용
                    string query = "SELECT idcard, phnumber FROM image LIMIT 1";

                    using (var command = new MySqlCommand(query, connection))
                    {
                        using (var reader = await command.ExecuteReaderAsync())
                        {
                            if (await reader.ReadAsync())
                            {
                                // BLOB 데이터를 가져와서 반환
                                var blobData = (byte[])reader["idcard"];
                                PH = reader["phnumber"].ToString();
                                return blobData;
                            }
                            else
                            {
                                throw new Exception("이미지 데이터가 존재하지 않습니다.");
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"오류 발생: {ex.Message}");
                    throw;
                }
            }
        }
    }
}
