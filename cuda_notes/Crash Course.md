Git - version control system 
- to track and save changes in our codebase 

###### terms 
1) directory 
2) terminal / command line
3) CLI - command line interface 
4) `cd` : change directory 
5) repository : project (or folder place where project is kept)
6) Git : tracks changes 
7) GitHub : website to host your repos online 
###### git commands
1) `clone` : bring online repo to local machine 
2) `add` : track changes 
3) `commit` : save the changes 
4) `push` : upload file to remote repo 
5) `pull` : download changes from remote repo to local machine

`git add .` : add both untracked and modified files 
###### GitHub Workflow vs Local Machine Workflow
```latex
1) write code 
2) commit changes 
3) pull request -> drawing to source repo 
```

```latex
1) write code
2) git add : save changes 
3) git commit : commit changes
4) git push : local machine -> remote repo
5) git pull : remote repo -> local machine 
```

##### Branching 
`master` : naming convention for the default branch 
- each branch only keeps track of the changes made in its own branch
- 