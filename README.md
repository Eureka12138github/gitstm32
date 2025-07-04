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

可以用`git restore <文件名>`来舍弃对该文件做出的修改。除非确实清楚不想要对这个文件所作出的修改了，否则不要使用这个指令

加入现在正在解决问题53，当前处于iss53分支上，原来的mainr代码突然出现了问题，需要马上修复。

此刻的做法是先切换回main分支，即`git switch main`,然后在此分支上新建一个`hotfix`分支用于修复紧急问题，修复完毕后，再切换回main，`git checkout main`
此后再使用`git merge hotfix`进行分支合并，如此main上的问题就被修复了。

当问题被修复后，可以使用`git branch -d hotfix`，删除该临时分支，然后再`git switch iss53`，继续解决问题53

如果`mian`分支和其他分支,如`iss53`分支先后对同一份文件进行修改，比如我现在编辑的内容是在`main`分支下进行的，暂存然后提交。后续如果`main`和`iss53`分支在没有合并的情况下，又切换到`iss53`分支下，
编辑此README.md的内容，（在`iss53分支下`应该是没有现在这段话的），编辑完毕后暂存提交，然后切换回`main`分支，这时候想要将`main`分支和`iss53`分支合并的话，就会出现冲突，需要手动解决，因为git无法判断究竟应该保留
哪部分内容（`main`和`iss53`同时对README.md进行了修改）。手动解决也不难，先用打开README.md，然后对其中内容进行取舍，之后再` git add * `，再之后` git commit -m 'fixed conflict'`就行了。
其实这个问题是可以避免的，那就是**先保持同步**，再修改内容。比如我在`main`中修改了README.md，应该先将`main`和`iss53`分支合并，之后如果在`iss53`分支下再修改README.md的内容的话，后续合并应该就不会出现冲突了。😎
