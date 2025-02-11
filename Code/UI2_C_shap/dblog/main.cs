using MySql.Data.MySqlClient;
using System;
using System.Windows.Forms;

namespace dblog
{
    public partial class main : Form
    {

        string connectionString = "Server=3.36.108.189; Database=idcard; Uid=test; Pwd=1234;"; // DB ���� ���ڿ�

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
                MessageBox.Show("���̵�� ��й�ȣ�� ��� �Է����ּ���.", "�Է� ����", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return; // �����̸� �Լ� ����
            }

            if (ValidateLogin(inputId, inputpw))
            {
                log Form3 = new log(this); // ���� ��(this)�� log ������ ����
                Form3.Show();
                this.Hide(); // ���� �� �����
            }
            else
            {
                MessageBox.Show("ID �Ǵ� ��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �õ��� �ּ���.", "�α��� ����", MessageBoxButtons.OK, MessageBoxIcon.Warning);
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

                    string query = "SELECT COUNT(*) FROM Manager WHERE id = @id AND pass = @password"; // Manager ���̺��� id�� pass �ʵ� Ȯ��
                    using (MySqlCommand command = new MySqlCommand(query, connection))
                    {
                        command.Parameters.AddWithValue("@id", inputId);
                        command.Parameters.AddWithValue("@password", inputPassword);

                        int userCount = Convert.ToInt32(command.ExecuteScalar()); // ����ڰ� ��ġ�ϸ� 1, �ƴϸ� 0

                        if (userCount > 0)
                        {
                            isValid = true; // ID�� ��й�ȣ�� ��ġ�ϴ� ���
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"�����ͺ��̽� ����: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
