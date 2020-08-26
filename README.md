<img src="logo/bustub.svg" alt="BusTub Logo" height="200">

-----------------

[![Build Status](https://travis-ci.org/cmu-db/bustub.svg?branch=master)](https://travis-ci.org/cmu-db/bustub)
[![CircleCI](https://circleci.com/gh/cmu-db/bustub/tree/master.svg?style=svg)](https://circleci.com/gh/cmu-db/bustub/tree/master)

BusTub is a relational database management system built at [Carnegie Mellon University](https://db.cs.cmu.edu) for the [Introduction to Database Systems](https://15445.courses.cs.cmu.edu) (15-445/645) course. This system was developed for educational purposes and should not be used in production environments.

**WARNING: IF YOU ARE A STUDENT IN THE CLASS, DO NOT DIRECTLY FORK THIS REPO. DO NOT PUSH PROJECT SOLUTIONS PUBLICLY. THIS IS AN ACADEMIC INTEGRITY VIOLATION AND CAN LEAD TO GETTING YOUR DEGREE REVOKED, EVEN AFTER YOU GRADUATE.**

## Cloning this repo

The following instructions will create a private BusTub that you can use for your development:

1. Go to [https://github.com/new](https://github.com/new) to create a new repo under your account. Pick a name (e.g. `private-bustub`) and make sure it is you select it as **private**.
2. On your development machine, clone the public BusTub:
   ```
   $ git clone --depth 1 git@github.com:cmu-db/bustub.git
   ```
3. You next need to [mirror](https://git-scm.com/docs/git-push#Documentation/git-push.txt---mirror) the public BusTub repo into your own private BusTub repo. Suppose your GitHub name is `student` and your repo name is `private-bustub`, you will execute the following commands:
   ```
   $ mkdir bustub
   $ cd bustub
   $ git push --mirror git@github.com:student/private-bustub.git
   ```
   This copies everything in the public BusTub repo into your own private repo. You can now delete this bustub directory:
   ```
   $ cd ..
   $ rm -rv bustub
   ```
4. Clone your own private repo on:
   ```
   $ git clone git@github.com:student/private-bustub.git
   ```
5. Add the public BusTub as a remote source. This will allow you to retrieve changes from the CMU-DB repository during the semester:
   ```
   $ git remote add public https://github.com/cmu-db/bustub.git
   ```
6. You can now pull in changes from the public BusTub as needed:
   ```
   $ git pull public master
   ```

We suggest working on your projects in separate branches. If you do not understand how Git branches work, [learn how](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging). If you fail to do this, you might lose all your work at some point in the semester, and nobody will be able to help you.

## Build

### Linux / Mac
To ensure that you have the proper packages installed on your machine, run `sudo build_support/packages.sh`. Then run

```
mkdir build
cd build
cmake ..
make
```
Debug mode:

```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
Debug build enables [AddressSanitizer](https://github.com/google/sanitizers), which can generate false positives for overflow on STL containers. If you encounter this, define the environment variable `ASAN_OPTIONS=detect_container_overflow=0`.

### Windows
If you are using a rather new version of Windows 10, you can use the Windows Subsystem for Linux (WSL) to develop, build, and test Bustub. All you need is to [Install WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10). You can just choose "Ubuntu" (no specific version) in Microsoft Store. Then, enter WSL and follow the above instructions.

If you are using CLion, it also [works with WSL](https://blog.jetbrains.com/clion/2018/01/clion-and-linux-toolchain-on-windows-are-now-friends).


## Testing
```
cd build
make check-tests
```


## TODO
* update: when size exceed that page, table heap returns false and delete/insert tuple (rid will change and need to delete/insert from index)
* delete empty page from table heap when delete tuple
* implement delete table, with empty page bitmap in disk manager (how to persistent?)
* index: unique/dup key, variable key
