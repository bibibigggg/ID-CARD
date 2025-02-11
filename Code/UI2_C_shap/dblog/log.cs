using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

namespace dblog
{
    public partial class log : Form
    {
        private Form mainForm;

        string connectionString = "Server=3.36.108.189; Database=idcard; Uid=test; Pwd=1234;"; // DB 연결

        private List<string> id = new List<string>(); // 아이디
        private List<string> names = new List<string>(); // 이름
        private List<string> pnumber = new List<string>(); // 주민등록번호
        private List<string> access_time = new List<string>(); // 접속시간

        // 라디오 버튼 인덱스 저장 변수
        private int selectedRowIndex = -1;

        public log(Form main)
        {
            InitializeComponent();
            mainForm = main; // 전달된 main 폼 저장
        }

        private void log_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (mainForm != null)
            {
                mainForm.Show(); // main 폼 다시 표시
            }
        }

        private void ConfigureTableLayoutPanel()
        {
            tableLayoutPanel.Controls.Clear();
            tableLayoutPanel.ColumnStyles.Clear();
            tableLayoutPanel.RowStyles.Clear();
            tableLayoutPanel.RowCount = 2; // 상단 제목 행 + 데이터 행들

            // 상단 제목 행 설정
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 30F)); // 제목 행 높이 고정
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));      // 데이터 행 자동 조정

            // 기본 열 구성 (5개 열)
            string[] defaultColumns = { "√", "순서", "이름", "주민등록번호", "접근 시간" };
            SetupColumns(defaultColumns);
        }

        private void SetupColumns(string[] columnNames)
        {
            tableLayoutPanel.Controls.Clear(); // 기존 컨트롤 삭제
            tableLayoutPanel.ColumnStyles.Clear(); // 기존 열 스타일 삭제

            int[] columnWidths = { 20, 50, 100, 210, 210 }; // 열 너비 고정

            tableLayoutPanel.ColumnCount = columnNames.Length;
            for (int i = 0; i < columnNames.Length; i++)
            {
                tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, columnWidths[i]));

                Label label = new Label
                {
                    Text = columnNames[i],
                    TextAlign = ContentAlignment.MiddleCenter,
                    BackColor = SystemColors.ControlLight,
                    Font = new Font(DefaultFont, FontStyle.Bold),
                    BorderStyle = BorderStyle.FixedSingle,
                    Margin = new Padding(0),
                    Size = new Size(columnWidths[i], 30)
                };

                tableLayoutPanel.Controls.Add(label, i, 0);
            }
        }

        private void bt_search_Click(object sender, EventArgs e)
        {
            selectedRowIndex = -1;

            // 테이블 초기화
            ResetTableData();

            // 검색어 가져오기
            string searchName = tb_search.Text.Trim(); // 검색 텍스트박스에 입력된 값

            if (string.IsNullOrEmpty(searchName))
            {
                MessageBox.Show("검색할 이름 또는 주민등록번호를 입력하세요.", "알림", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            // 데이터 가져오기 (검색 조건 적용)
            LoadDataFromDatabase(searchName);
        }

        private void ResetTableData()
        {
            tableLayoutPanel.Controls.Clear(); // 모든 컨트롤 삭제
            tableLayoutPanel.ColumnStyles.Clear(); // 열 스타일 초기화
            tableLayoutPanel.RowStyles.Clear(); // 행 스타일 초기화
        }

        private void LoadDataFromDatabase(string searchText)
        {
            try
            {
                using (MySqlConnection connection = new MySqlConnection(connectionString))
                {
                    connection.Open();

                    // 이름과 주민등록번호 앞자리를 포함하여 검색
                    string query = @"
                        SELECT id, name, pnumber, access_time
                        FROM log
                        WHERE name LIKE @searchText OR pnumber LIKE @searchText";

                    using (MySqlCommand command = new MySqlCommand(query, connection))
                    {
                        // @searchText 매개변수에 "%"를 붙여서 부분 일치를 찾음
                        command.Parameters.AddWithValue("@searchText", $"%{searchText}%");

                        using (MySqlDataAdapter adapter = new MySqlDataAdapter(command))
                        {
                            DataTable idCardData = new DataTable();
                            adapter.Fill(idCardData);

                            id.Clear();
                            names.Clear();
                            pnumber.Clear();
                            access_time.Clear();

                            foreach (DataRow row in idCardData.Rows)
                            {
                                id.Add(row["id"].ToString());
                                names.Add(row["name"].ToString());
                                pnumber.Add(row["pnumber"].ToString());
                                access_time.Add(row["access_time"].ToString());
                            }
                        }
                    }
                }

                if (names.Count == 0)
                {
                    MessageBox.Show("검색 결과가 없습니다.", "알림", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                ConfigureTableLayoutPanel();
                PopulateTableLayoutPanel();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"데이터베이스 오류: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void PopulateTableLayoutPanel()
        {
            int rowHeight = 25;
            int dataCount = names.Count;

            int[] columnWidths = { 20, 50, 100, 210, 210 }; // 열 너비 고정

            tableLayoutPanel.RowCount = dataCount + 1;

            for (int i = 0; i < dataCount; i++)
            {
                var radioButton = new RadioButton
                {
                    AutoSize = true,
                    Dock = DockStyle.Top,
                    Tag = i // 인덱스를 태그로 저장
                };

                radioButton.CheckedChanged += (sender, e) =>
                {
                    if (radioButton.Checked) // 체크된 경우만 동작
                    {
                        selectedRowIndex = (int)radioButton.Tag; // 태그에서 인덱스를 가져옴
                    }
                };

                tableLayoutPanel.Controls.Add(radioButton, 0, i + 1);

                var orderLabel = new Label
                {
                    Text = (i + 1).ToString(),
                    AutoSize = false,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Size = new Size(columnWidths[1], rowHeight) // 충분한 너비 지정
                };
                tableLayoutPanel.Controls.Add(orderLabel, 1, i + 1);

                var nameLabel = new Label
                {
                    Text = names[i],
                    AutoSize = false,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Size = new Size(columnWidths[2], rowHeight) // 충분한 너비 지정
                };
                tableLayoutPanel.Controls.Add(nameLabel, 2, i + 1);

                var idLabel = new Label
                {
                    Text = pnumber[i],
                    AutoSize = false,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Size = new Size(columnWidths[3], rowHeight) // 충분한 너비 지정
                };
                tableLayoutPanel.Controls.Add(idLabel, 3, i + 1);

                var accessLabel = new Label
                {
                    Text = access_time[i],
                    AutoSize = false,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Size = new Size(columnWidths[4], rowHeight) // 충분한 너비 지정
                };
                tableLayoutPanel.Controls.Add(accessLabel, 4, i + 1);
            }
        }

        private void btn_log_del_Click(object sender, EventArgs e)
        {
            try
            {
                using (MySqlConnection connection = new MySqlConnection(connectionString))
                {
                    connection.Open();

                    // 현재 로그 테이블의 레코드 개수 확인
                    string countQuery = "SELECT COUNT(*) FROM log";
                    using (MySqlCommand countCommand = new MySqlCommand(countQuery, connection))
                    {
                        int recordCount = Convert.ToInt32(countCommand.ExecuteScalar());

                        if (recordCount == 0)
                        {
                            // 로그가 없을 경우 메시지 출력
                            MessageBox.Show("출입 기록이 없습니다.", "알림", MessageBoxButtons.OK, MessageBoxIcon.Information);
                            return;
                        }
                    }

                    // 로그 삭제 확인
                    var result = MessageBox.Show("정말로 모든 데이터를 삭제하시겠습니까?", "확인", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                    if (result == DialogResult.Yes)
                    {
                        // 테이블 로그 삭제
                        string deleteQuery = "TRUNCATE TABLE log";
                        using (MySqlCommand deleteCommand = new MySqlCommand(deleteQuery, connection))
                        {
                            deleteCommand.ExecuteNonQuery();
                        }

                        MessageBox.Show("모든 로그가 삭제되었습니다.", "삭제 완료", MessageBoxButtons.OK, MessageBoxIcon.Information);

                        // 테이블 UI 초기화
                        tableLayoutPanel.Controls.Clear();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"오류 발생: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void bt_choice_Click(object sender, EventArgs e)
        {
            if (selectedRowIndex < 0 || selectedRowIndex >= names.Count)
            {
                MessageBox.Show("선택된 사용자가 없습니다.", "알림", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            string selectedName = names[selectedRowIndex];
            byte[] faceData = null;

            try
            {
                // 데이터베이스에서 해당 사용자의 얼굴 데이터(BLOB) 가져오기
                using (MySqlConnection connection = new MySqlConnection(connectionString))
                {
                    connection.Open();

                    string query = "SELECT face_data FROM log WHERE name = @name";
                    using (MySqlCommand command = new MySqlCommand(query, connection))
                    {
                        command.Parameters.AddWithValue("@name", selectedName);

                        using (MySqlDataReader reader = command.ExecuteReader())
                        {
                            if (reader.Read())
                            {
                                faceData = (byte[])reader["face_data"];
                            }
                        }
                    }
                }

                if (faceData != null)
                {
                    ShowImage(faceData); // 바이너리 데이터를 통해 이미지 표시
                }
                else
                {
                    MessageBox.Show("해당 사용자의 얼굴 데이터를 찾을 수 없습니다.", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"데이터베이스 오류: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        // 얼굴 이미지를 바이너리 데이터에서 표시
        private void ShowImage(byte[] imageData)
        {
            try
            {
                using (var ms = new MemoryStream(imageData))
                {
                    Form imageForm = new Form
                    {
                        Text = "사용자 얼굴 사진",
                        Size = new Size(400, 400),
                        StartPosition = FormStartPosition.CenterScreen // 창을 화면 가운데에 표시
                    };

                    PictureBox pictureBox = new PictureBox
                    {
                        Image = Image.FromStream(ms),
                        SizeMode = PictureBoxSizeMode.Zoom,
                        Dock = DockStyle.Fill
                    };

                    imageForm.Controls.Add(pictureBox);
                    imageForm.ShowDialog();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"이미지를 표시하는 중 오류가 발생했습니다: {ex.Message}", "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void Form3_Load(object sender, EventArgs e)
        {
            ResetTableData(); // 초기화
            this.FormClosed += log_FormClosed; // FormClosed 이벤트 등록
        }
    }
}
