<img src="logo/bustub.svg" alt="BusTub Logo" height="200">

-----------------

BusTub is a relational database management system built at Carnegie Mellon University for the [Intro to Database Systems](https://15445.courses.cs.cmu.edu) (15-445/645) course. This system was developed for educational purposes and should not be used in production environments.

## Build
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
