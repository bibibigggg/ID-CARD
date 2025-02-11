using MySql.Data.MySqlClient;
using System;
using System.Windows.Forms;

namespace dblog
{
    public partial class main : Form
    {

        string connectionString = "Server=3.36.108.189; Database=idcard; Uid=test; Pwd=1234;"; // DB 연결 문자열

        public main()
        {
            InitializeComponent();
        }

        private void main_Load(object sender, EventArgs e)
        {

        }

        private void btn_login_Click(object sender, EventArgs e)
        {
            string inputId = tb_id.Text;
            string inputpw = tb_pw.Text;

            if (string.IsNullOrEmpty(inputId) || string.IsNullOrEmpty(inputpw))
            {
                MessageBox.Show("아이디와 비밀번호를 모두 입력해주세요.", "입력 오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return; // 공란이면 함수 종료
            }

            if (ValidateLogin(inputId, inputpw))
            {
                log Form3 = new log(this); // 현재 폼(this)을 log 폼으로 전달
                Form3.Show();
                this.Hide(); // 현재 폼 숨기기
            }
            else
            {
                MessageBox.Show("ID 또는 비밀번호가 일치하지 않습니다. 다시 시도해 주세요.", "로그인 실패", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private bool ValidateLogin(string inputId, string inputPassword)
        {
            bool isValid = false;

            try
            {
                using (MySqlConnection connection = new MySqlConnection(connectionString))
                {
                    connection.Open();

                    string query = "SELECT COUNT(*) FROM Manager WHERE id = @id AND pass = @password"; // Manager 테이블의 id와 pass 필드 확인
                    using (MySqlCommand command = new MySqlCommand(query, connection))
                    {
                        command.Parameters.AddWithValue("@id", inputId);
                        command.Parameters.AddWithValue("@password", inputPassword);

                        int userCount = Convert.ToInt32(command.ExecuteScalar()); // 사용자가 일치하면 1, 아니면 0

                        if (userCount > 0)
                        {
                            isValid = true; // ID와 비밀번호가 일치하는 경우
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"데이터베이스 오류: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return isValid;
        }

        private void btn_member_Click(object sender, EventArgs e)
        {
            member member = new member(this);
            member.Show();
            this.Hide();
        }
    }
}
