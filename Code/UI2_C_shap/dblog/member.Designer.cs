namespace dblog
{
    partial class member
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(member));
            lb_name = new Label();
            lb_id = new Label();
            lb_pw1 = new Label();
            lb_pw2 = new Label();
            tb_name = new TextBox();
            tb_id = new TextBox();
            tb_pw1 = new TextBox();
            tb_pw2 = new TextBox();
            btn_id_dup = new Button();
            lb_pw2_re = new Label();
            lb_pw1_re = new Label();
            btn_create = new Button();
            SuspendLayout();
            // 
            // lb_name
            // 
            lb_name.AutoSize = true;
            lb_name.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            lb_name.ForeColor = Color.Black;
            lb_name.Location = new Point(100, 33);
            lb_name.Name = "lb_name";
            lb_name.Size = new Size(42, 21);
            lb_name.TabIndex = 0;
            lb_name.Text = "이름";
            // 
            // lb_id
            // 
            lb_id.AutoSize = true;
            lb_id.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            lb_id.ForeColor = Color.Black;
            lb_id.Location = new Point(84, 93);
            lb_id.Name = "lb_id";
            lb_id.Size = new Size(58, 21);
            lb_id.TabIndex = 1;
            lb_id.Text = "아이디";
            // 
            // lb_pw1
            // 
            lb_pw1.AutoSize = true;
            lb_pw1.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            lb_pw1.ForeColor = Color.Black;
            lb_pw1.Location = new Point(68, 153);
            lb_pw1.Name = "lb_pw1";
            lb_pw1.Size = new Size(74, 21);
            lb_pw1.TabIndex = 2;
            lb_pw1.Text = "비밀번호";
            // 
            // lb_pw2
            // 
            lb_pw2.AutoSize = true;
            lb_pw2.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            lb_pw2.ForeColor = Color.Black;
            lb_pw2.Location = new Point(30, 213);
            lb_pw2.Name = "lb_pw2";
            lb_pw2.Size = new Size(112, 21);
            lb_pw2.TabIndex = 3;
            lb_pw2.Text = "비밀번호 확인";
            // 
            // tb_name
            // 
            tb_name.BorderStyle = BorderStyle.FixedSingle;
            tb_name.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            tb_name.Location = new Point(156, 30);
            tb_name.Name = "tb_name";
            tb_name.Size = new Size(340, 27);
            tb_name.TabIndex = 4;
            // 
            // tb_id
            // 
            tb_id.BorderStyle = BorderStyle.FixedSingle;
            tb_id.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            tb_id.Location = new Point(156, 90);
            tb_id.Name = "tb_id";
            tb_id.Size = new Size(340, 27);
            tb_id.TabIndex = 5;
            // 
            // tb_pw1
            // 
            tb_pw1.BorderStyle = BorderStyle.FixedSingle;
            tb_pw1.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            tb_pw1.Location = new Point(156, 150);
            tb_pw1.Name = "tb_pw1";
            tb_pw1.Size = new Size(340, 27);
            tb_pw1.TabIndex = 6;
            // 
            // tb_pw2
            // 
            tb_pw2.BorderStyle = BorderStyle.FixedSingle;
            tb_pw2.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            tb_pw2.Location = new Point(156, 210);
            tb_pw2.Name = "tb_pw2";
            tb_pw2.Size = new Size(340, 27);
            tb_pw2.TabIndex = 7;
            tb_pw2.TextChanged += tb_pw2_TextChanged;
            // 
            // btn_id_dup
            // 
            btn_id_dup.BackColor = Color.White;
            btn_id_dup.FlatStyle = FlatStyle.Flat;
            btn_id_dup.Font = new Font("맑은 고딕", 9.75F, FontStyle.Regular, GraphicsUnit.Point);
            btn_id_dup.Location = new Point(505, 91);
            btn_id_dup.Name = "btn_id_dup";
            btn_id_dup.Size = new Size(120, 25);
            btn_id_dup.TabIndex = 8;
            btn_id_dup.Text = "아이디 중복 확인";
            btn_id_dup.UseVisualStyleBackColor = false;
            btn_id_dup.Click += btn_id_dup_Click;
            // 
            // lb_pw2_re
            // 
            lb_pw2_re.Font = new Font("맑은 고딕", 9.75F, FontStyle.Regular, GraphicsUnit.Point);
            lb_pw2_re.ForeColor = Color.Black;
            lb_pw2_re.Location = new Point(156, 240);
            lb_pw2_re.Name = "lb_pw2_re";
            lb_pw2_re.Size = new Size(201, 17);
            lb_pw2_re.TabIndex = 9;
            lb_pw2_re.Text = "* 비밀번호가 일치하지 않습니다.";
            // 
            // lb_pw1_re
            // 
            lb_pw1_re.Font = new Font("맑은 고딕", 9.75F, FontStyle.Regular, GraphicsUnit.Point);
            lb_pw1_re.ForeColor = Color.Black;
            lb_pw1_re.Location = new Point(156, 180);
            lb_pw1_re.Name = "lb_pw1_re";
            lb_pw1_re.Size = new Size(338, 17);
            lb_pw1_re.TabIndex = 10;
            lb_pw1_re.Text = "* 영문, 숫자, 특수문자를 반드시 1개씩 포함해야 합니다.";
            // 
            // btn_create
            // 
            btn_create.FlatStyle = FlatStyle.Flat;
            btn_create.Font = new Font("맑은 고딕", 12F, FontStyle.Regular, GraphicsUnit.Point);
            btn_create.Location = new Point(277, 270);
            btn_create.Name = "btn_create";
            btn_create.Size = new Size(100, 30);
            btn_create.TabIndex = 11;
            btn_create.Text = "생성하기";
            btn_create.UseVisualStyleBackColor = true;
            btn_create.Click += btn_create_Click;
            // 
            // member
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            BackColor = Color.White;
            ClientSize = new Size(654, 321);
            Controls.Add(btn_create);
            Controls.Add(lb_pw1_re);
            Controls.Add(lb_pw2_re);
            Controls.Add(btn_id_dup);
            Controls.Add(tb_pw2);
            Controls.Add(tb_pw1);
            Controls.Add(tb_id);
            Controls.Add(tb_name);
            Controls.Add(lb_pw2);
            Controls.Add(lb_pw1);
            Controls.Add(lb_id);
            Controls.Add(lb_name);
            Icon = (Icon)resources.GetObject("$this.Icon");
            Name = "member";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "membership";
            Load += member_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label lb_name;
        private Label lb_id;
        private Label lb_pw1;
        private Label lb_pw2;
        private TextBox tb_name;
        private TextBox tb_id;
        private TextBox tb_pw1;
        private TextBox tb_pw2;
        private Button btn_id_dup;
        private Label lb_pw2_re;
        private Label lb_pw1_re;
        private Button btn_create;
    }
}