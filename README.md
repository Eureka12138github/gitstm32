STM32 学习记录仓库
这是一个用于记录学习 STM32 的过程的仓库。
当前尝试使用命令行工具（而非 Sourcetree）进行代码版本管理与远程推送操作。
Git 使用技巧汇总
修改 README
😊😊😊
测试 git diff 功能
git diff：查看未暂存文件与上次提交之间的差异。
git diff --staged：查看已暂存文件与上次提交之间的差异。
✅ 一句话总结：如果文件在未暂存区，使用 git diff；如果文件已暂存，使用 git diff --staged。
提交操作
git commit -a：跳过 git add，一键暂存并提交所有修改。
git commit --amend：撤销最近一次提交，并允许：
使用 git add 补充遗漏的文件；
编辑或保留原有的提交信息后重新提交。
🔁 适用于刚提交完发现还有文件没添加，或者需要修改提交信息的情况。
文件重命名操作
git mv <原文件名> <新文件名>：用于重命名 Git 跟踪的文件。
例：
git mv test1.txt test.txt
撤销暂存操作
git reset HEAD <文件名>：取消某个文件的暂存状态。
示例：
git reset HEAD README.md
📌 常用于误将某些文件加入暂存区后想将其移出的情况。
放弃本地修改
git restore <文件名>：放弃对该文件的所有本地修改。
⚠️ 注意：该操作不可逆，请确保确实不需要这些修改后再执行。
