<img src="logo/bustub.svg" alt="BusTub Logo" height="200">

-----------------

[![Build Status](https://travis-ci.org/cmu-db/bustub.svg?branch=master)](https://travis-ci.org/cmu-db/bustub)
[![CircleCI](https://circleci.com/gh/cmu-db/bustub/tree/master.svg?style=svg)](https://circleci.com/gh/cmu-db/bustub/tree/master)

BusTub is a relational database management system built at [Carnegie Mellon University](https://db.cs.cmu.edu) for the [Introduction to Database Systems](https://15445.courses.cs.cmu.edu) (15-445/645) course. This system was developed for educational purposes and should not be used in production environments.

# WARNING: DO NOT DIRECTLY FORK THIS REPO! IT'S GONNA BE PUBLIC!

Please follow [the directions](<https://medium.com/@bilalbayasut/github-how-to-make-a-fork-of-public-repository-private-6ee8cacaf9d3>) to "make a private fork" in your account if you want. Generally, it's fine as long as you can pull the latest changes from this repo, and your repo is not visible to others. We don't care where your repo is. 

After you made your own repo, please add this repo as a remote:

`git remote add public https://github.com/cmu-db/bustub.git`

And then you can pull the latest update to your `master` branch by

`git pull public master`

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
