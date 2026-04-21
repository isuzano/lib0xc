# Contributing to lib0xc

Thank you for your interest in contributing to lib0xc!

## Contributor License Agreement

This project welcomes contributions and suggestions. Most contributions require
you to agree to a Contributor License Agreement (CLA) declaring that you have
the right to, and actually do, grant us the rights to use your contribution.
For details, visit <https://cla.opensource.microsoft.com>.

When you submit a pull request, a CLA bot will automatically determine whether
you need to provide a CLA and decorate the PR appropriately (e.g., status
check, comment). Simply follow the instructions provided by the bot. You will
only need to do this once across all repos using our CLA.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct][coc]. For
more information see the [Code of Conduct FAQ][coc-faq] or contact
[opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional
questions or comments.

[coc]: https://opensource.microsoft.com/codeofconduct/
[coc-faq]: https://opensource.microsoft.com/codeofconduct/faq/

## How to Contribute

1. Fork the repository and create a topic branch from `main`.
2. Make your changes, following the conventions and style of the surrounding
   code.
3. Add or update unit tests under `0xtest/unit/` for any new functionality.
4. Ensure `make test` passes on your platform.
5. Submit a pull request against `main`.

### Source File Headers

All new source files must begin with the following header:

```c
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
```

### Reporting Issues

Please use GitHub issues to report bugs and request features. For security
vulnerabilities, see [SECURITY.md](SECURITY.md) — **do not** open a public
issue.
