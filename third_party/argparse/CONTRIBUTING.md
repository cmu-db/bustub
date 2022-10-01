# Contributing
Contributions are welcomed. Open a pull-request or an issue.

## Code of conduct
This project adheres to the [Open Code of Conduct][code-of-conduct]. By participating, you are expected to honor this code.

[code-of-conduct]: https://github.com/spotify/code-of-conduct/blob/master/code-of-conduct.md

## Code Style

This project prefers, but does not strictly enforce, a specific source code style. The style is described in `.clang-format` and `.clang-tidy`.

To generate a clang-tidy report:

```bash
clang-tidy --extra-arg=-std=c++17 --config-file=.clang-tidy include/argparse/argparse.hpp
```
