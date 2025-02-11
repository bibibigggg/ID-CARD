using MySql.Data.MySqlClient;
using Org.BouncyCastle.Asn1.Cmp;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace dblog
{
    public partial class member : Form
    {
        private Form mainForm;

        string connectionString = "Server=3.36.108.189; Database=idcard; Uid=test; Pwd=1234;"; // DB 연결 문자열

        private bool isIdChecked = false; // 아이디 중복 확인 여부를 저장하는 플래그

        public member(Form main)
        {
            InitializeComponent();
            mainForm = main; // 전달된 main 폼 저장
        }

        private void member_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (mainForm != null)
            {
                mainForm.Show(); // main 폼 다시 표시
            }
        }

        private void btn_id_dup_Click(object sender, EventArgs e)
        {
            string inputId = tb_id.Text.Trim();

            if (string.IsNullOrEmpty(inputId))
            {
                MessageBox.Show("아이디를 입력해주세요.");
                return;
            }

            try
            {
                // MySqlConnection 객체를 사용하여 DB에 연결
                using (var connection = new MySql.Data.MySqlClient.MySqlConnection(connectionString))
                {
                    connection.Open();

                    // 아이디 중복 확인을 위한 SQL 쿼리
                    string query = "SELECT COUNT(*) From Manager WHERE id = @id";

                    // MySqlCommand 객체를 생성하고 쿼리 실행
                    using (var command = new MySql.Data.MySqlClient.MySqlCommand(query, connection))
                    {
                        // 매개변수로 입력된 아이디 값을 전달
                        command.Parameters.AddWithValue("@id", inputId);

                        // 쿼리 실행 결과를 얻음
                        int count = Convert.ToInt32(command.ExecuteScalar());

                        // count가 0이면 사용 가능한 아이디, 그렇지 않으면 중복된 아이디
                        if (count == 0)
                        {
                            MessageBox.Show("사용 가능한 아이디 입니다.");
                        }
                        else
                        {
                            MessageBox.Show("이미 존재하는 아이디 입니다.");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                // 예외 발생 시 에러 메시지를 표시
                MessageBox.Show("오류가 발생했습니다 : " + ex.Message);
            }
        }

        private void btn_create_Click(object sender, EventArgs e)
        {
            // 텍스트 박스 값 가져오기
            string userId = tb_id.Text.Trim();
            string userName = tb_name.Text.Trim();
            string password1 = tb_pw1.Text.Trim();
            string password2 = tb_pw2.Text.Trim();

            // lb_pw1_re, lb_pw2_re 레이블 초기화
            lb_pw1_re.Text = "";
            lb_pw2_re.Text = "";

            // 모든 텍스트 박스가 채워졌는지 확인
            if (string.IsNullOrEmpty(userId) || string.IsNullOrEmpty(userName) || string.IsNullOrEmpty(password1) || string.IsNullOrEmpty(password2))
            {
                MessageBox.Show("작성하지 않은 곳이 있습니다. 확인해주세요.");
                return;
            }

            bool isPasswordValid = true;

            // 비밀번호 규칙 체크 : 영문, 숫자, 특수문자 각각 1개 이상 포함
            if (!password1.Any(char.IsLetter) || !password1.Any(char.IsDigit) || !password1.Any(ch => !Char.IsLetterOrDigit(ch)))
            {
                lb_pw1_re.Text = "* 영문, 숫자, 특수문자를 반드시 1개씩 포함해야 합니다.";

                isPasswordValid = false;
            }

            // 비밀번호 일치 체크
            if (password1 != password2)
            {
                lb_pw2_re.Text = "* 비밀번호가 일치하지 않습니다.";

                isPasswordValid = false;
            }

            // 비밀번호 유효 & 일치 시 DB에 저장
            if (isPasswordValid)
            {
                try
                {
                    // DB 객체 사용, DB 연결
                    using (var connection = new MySql.Data.MySqlClient.MySqlConnection(connectionString))
                    {
                        connection.Open();

                        // DB에 아이디, 비밀번호, 이름을 삽입하는 SQL 쿼리
                        string query = "INSERT INTO Manager (id, pass, name) VALUES (@id, @pass, @name)";

                        using (var command = new MySql.Data.MySqlClient.MySqlCommand(query, connection))
                        {
                            // SQL 매개 변수에 값 할당
                            command.Parameters.AddWithValue("@id", userId);
                            command.Parameters.AddWithValue("@pass", password1);
                            command.Parameters.AddWithValue("@name", userName);

                            // SQL 쿼리 실행
                            command.ExecuteNonQuery();
                        }
                    }

                    // 저장 성공 후 창 닫기
                    MessageBox.Show("회원가입이 완료되었습니다.");
                    this.Close();
                }

                catch (Exception ex)
                {
                    MessageBox.Show("오류가 발생했습니다 : " + ex.Message);
                }
            }
        }

        private void tb_pw1_TextChanged(object sender, EventArgs e)
        {
            // 비밀번호 규칙 확인
            if (!tb_pw1.Text.Any(char.IsLetter) || !tb_pw1.Text.Any(char.IsDigit) || !tb_pw1.Text.Any(ch => !Char.IsLetterOrDigit(ch)))
            {
                lb_pw1_re.Text = "* 영문, 숫자, 특수문자를 반드시 1개씩 포함해야 합니다.";
            }
            else
            {
                lb_pw1_re.Text = "* 사용 가능한 비밀번호입니다.";
            }

            // 비밀번호 일치 여부 확인
            CheckPasswordMatch();
        }

        private void tb_pw2_TextChanged(object sender, EventArgs e)
        {
            // 비밀번호 일치 여부 확인
            CheckPasswordMatch();
        }

        private void CheckPasswordMatch()
        {
            if (tb_pw1.Text == tb_pw2.Text)
            {
                lb_pw2_re.Text = "* 비밀번호가 일치합니다.";
            }
            else
            {
                lb_pw2_re.Text = "* 비밀번호가 일치하지 않습니다.";
            }
        }

        private void member_Load(object sender, EventArgs e)
        {
            this.FormClosed += member_FormClosed; // FormClosed 이벤트 등록
        }
    }
}