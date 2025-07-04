这是一个学习stm32的记录仓库
现在尝试使用命令行而非Sourcetree将代码更新到远端。
### 修改README
😊😊😊
测试git drff
`git diff`用于查看未暂存文件与先前文件的差别
`git diff --staged`用于查看已暂存文件与先前文件的差别
一句话：如果文件在**未暂存区**则使用`git diff`；如果文件**已暂存**则使用`git diff --staged`
现在尝试使用`git commit -a`指令，该指令可以跳过`git add`，相当于一键暂存与提交
修改文件名称：`git mv`
例如如果我想把`test1.txt`修改为`test.txt`，就可以用`git mv test1.txt test.txt`
使用`git commit --amend`可以撤销上次修改，之后可以使用`git add`重新添加文件，然后再使用`git commit`提交
这时候可以“继承”上次的修改信息，可以对其再次编辑，最后再提交，这样提交快照不变，但增添了新内容。适用于刚提交完一次后，发现还有文件没有暂存或者说需要修改提交信息的情况。

可以用`git reset HEAD <文件名>`来取消暂存，比如先是使用了`git add *`暂存了所有文件，但是这时候还不需要暂存README.md文件，就可以使用 `git reset HEAD README.md` 就可以取消暂存了。
可以用`git restore <文件名>`来舍弃对该文件做出的修改。除非确实清楚不想要对这个文件所作出的修改了，否则不要使用这个指令，
