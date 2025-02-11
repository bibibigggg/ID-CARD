namespace dblog
{
    partial class main
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(main));
            btn_login = new Button();
            btn_member = new Button();
            tb_id = new TextBox();
            tb_pw = new TextBox();
            SuspendLayout();
            // 
            // btn_login
            // 
            btn_login.BackColor = Color.White;
            btn_login.FlatStyle = FlatStyle.Flat;
            btn_login.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            btn_login.Location = new Point(130, 230);
            btn_login.Name = "btn_login";
            btn_login.Size = new Size(100, 40);
            btn_login.TabIndex = 0;
            btn_login.Text = "로그인";
            btn_login.UseVisualStyleBackColor = false;
            btn_login.Click += btn_login_Click;
            // 
            // btn_member
            // 
            btn_member.BackColor = Color.White;
            btn_member.FlatStyle = FlatStyle.Flat;
            btn_member.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            btn_member.Location = new Point(255, 230);
            btn_member.Name = "btn_member";
            btn_member.Size = new Size(100, 40);
            btn_member.TabIndex = 1;
            btn_member.Text = "회원가입";
            btn_member.UseVisualStyleBackColor = false;
            btn_member.Click += btn_member_Click;
            // 
            // tb_id
            // 
            tb_id.BorderStyle = BorderStyle.FixedSingle;
            tb_id.Location = new Point(130, 160);
            tb_id.Name = "tb_id";
            tb_id.Size = new Size(225, 23);
            tb_id.TabIndex = 3;
            // 
            // tb_pw
            // 
            tb_pw.BorderStyle = BorderStyle.FixedSingle;
            tb_pw.Location = new Point(130, 190);
            tb_pw.Name = "tb_pw";
            tb_pw.PasswordChar = '*';
            tb_pw.Size = new Size(225, 23);
            tb_pw.TabIndex = 4;
            // 
            // main
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            BackColor = Color.White;
            BackgroundImage = Properties.Resources.main5;
            ClientSize = new Size(484, 311);
            Controls.Add(tb_pw);
            Controls.Add(tb_id);
            Controls.Add(btn_member);
            Controls.Add(btn_login);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            Icon = (Icon)resources.GetObject("$this.Icon");
            Name = "main";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "main";
            Load += main_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button btn_login;
        private Button btn_member;
        private TextBox tb_id;
        private TextBox tb_pw;
    }
}
