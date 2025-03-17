# Contributing to Tuw

Tuw is an open-source project that warmly welcomes contributions. We appreciate your help in improving the project!

## Coding Style

Tuw follows [Google's C++ Style Guide](http://google.github.io/styleguide/cppguide.html) with the following modifications:

- Line length should not exceed 100 characters.
- Use 4 spaces for indentation.
- No copyright notice or author line is required in source files.
- `std` c++ library is prohibited.
- Use [`noex`](../include/noex) library if you need strings and vectors.
- All functions should have `noexcept` specifiers.

## CI Workflow

No PR is merged until it passes the following code checks:

- Linting by cpplint.py 1.6
- Typo check by [codespell](https://github.com/codespell-project/codespell)
- Unit tests with `meson test -C build`

Before sending PRs, it is recommended to run tests and linting using the following commands.

```sh
meson setup build
meson compile -C build
meson test -C build -v
cpplint --recursive --quiet .
codespell -S "build,subprojects"
```

You can also run [test.yml](../.github/workflows/test.yml) manually on your forked repo.

[Manually running a workflow - GitHub Docs](https://docs.github.com/en/actions/managing-workflow-runs-and-deployments/managing-workflow-runs/manually-running-a-workflow)
