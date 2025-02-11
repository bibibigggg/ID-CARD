namespace dblog
{
    partial class log
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(log));
            tb_search = new TextBox();
            button2 = new Button();
            bt_search = new Button();
            tableLayoutPanel = new TableLayoutPanel();
            bt_choice = new Button();
            panel1 = new Panel();
            btn_log_del = new Button();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // tb_search
            // 
            tb_search.BorderStyle = BorderStyle.FixedSingle;
            tb_search.Font = new Font("맑은 고딕", 9F, FontStyle.Regular, GraphicsUnit.Point);
            tb_search.Location = new Point(20, 20);
            tb_search.Name = "tb_search";
            tb_search.Size = new Size(318, 23);
            tb_search.TabIndex = 0;
            // 
            // button2
            // 
            button2.Location = new Point(666, 26);
            button2.Name = "button2";
            button2.Size = new Size(0, 0);
            button2.TabIndex = 2;
            button2.Text = "button2";
            button2.UseVisualStyleBackColor = true;
            // 
            // bt_search
            // 
            bt_search.FlatStyle = FlatStyle.Flat;
            bt_search.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            bt_search.Location = new Point(344, 15);
            bt_search.Name = "bt_search";
            bt_search.Size = new Size(85, 30);
            bt_search.TabIndex = 4;
            bt_search.Text = "검색";
            bt_search.UseVisualStyleBackColor = true;
            bt_search.Click += bt_search_Click;
            // 
            // tableLayoutPanel
            // 
            tableLayoutPanel.Anchor = AnchorStyles.None;
            tableLayoutPanel.AutoScroll = true;
            tableLayoutPanel.AutoSizeMode = AutoSizeMode.GrowAndShrink;
            tableLayoutPanel.ColumnCount = 5;
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 20F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 50F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 100F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 210F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 210F));
            tableLayoutPanel.Font = new Font("맑은 고딕", 9.75F, FontStyle.Regular, GraphicsUnit.Point);
            tableLayoutPanel.Location = new Point(0, 0);
            tableLayoutPanel.Margin = new Padding(0);
            tableLayoutPanel.Name = "tableLayoutPanel";
            tableLayoutPanel.RowCount = 1;
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 100F));
            tableLayoutPanel.Size = new Size(590, 255);
            tableLayoutPanel.TabIndex = 5;
            // 
            // bt_choice
            // 
            bt_choice.FlatStyle = FlatStyle.Flat;
            bt_choice.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            bt_choice.Location = new Point(435, 15);
            bt_choice.Name = "bt_choice";
            bt_choice.Size = new Size(85, 30);
            bt_choice.TabIndex = 9;
            bt_choice.Text = "인물 확인";
            bt_choice.UseVisualStyleBackColor = true;
            bt_choice.Click += bt_choice_Click;
            // 
            // panel1
            // 
            panel1.AutoScroll = true;
            panel1.Controls.Add(tableLayoutPanel);
            panel1.Location = new Point(20, 55);
            panel1.Name = "panel1";
            panel1.Size = new Size(590, 255);
            panel1.TabIndex = 10;
            // 
            // btn_log_del
            // 
            btn_log_del.BackColor = Color.White;
            btn_log_del.FlatStyle = FlatStyle.Flat;
            btn_log_del.Font = new Font("맑은 고딕", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            btn_log_del.Location = new Point(526, 15);
            btn_log_del.Name = "btn_log_del";
            btn_log_del.Size = new Size(85, 30);
            btn_log_del.TabIndex = 12;
            btn_log_del.Text = "로그 삭제";
            btn_log_del.UseVisualStyleBackColor = false;
            btn_log_del.Click += btn_log_del_Click;
            // 
            // log
            // 
            AutoScaleDimensions = new SizeF(8F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = Color.White;
            ClientSize = new Size(629, 331);
            Controls.Add(btn_log_del);
            Controls.Add(bt_choice);
            Controls.Add(bt_search);
            Controls.Add(button2);
            Controls.Add(tb_search);
            Controls.Add(panel1);
            Font = new Font("Cafe24 Ohsquare air Light", 9F, FontStyle.Regular, GraphicsUnit.Point);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            Icon = (Icon)resources.GetObject("$this.Icon");
            Name = "log";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "DB Log";
            Load += Form3_Load;
            panel1.ResumeLayout(false);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private System.Windows.Forms.TextBox tb_search;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button bt_search;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
        private System.Windows.Forms.Button bt_choice;
        private System.Windows.Forms.Panel panel1;
        private Button btn_log_del;
    }
}