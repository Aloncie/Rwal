# Contributing to Rwal

Rwal is a small project, so the rules are simple.

## Getting started

1. Clone the repo:
   ```bash
   git clone https://github.com/Aloncie/Rwal.git
   cd Rwal
   ```
2. Install dependencies and build:
   ```bash
   python setup.py
   python build.py
   ```
3. Run the binary to verify everything works:
   ```bash
   ./build/<OS>-release/rwal --help
   ```

## Sending changes

- Fork the repository and create a feature branch.
- Follow the existing code style: C++20, RAII, smart pointers, dependency injection through interfaces.
- Verify formatting after changes: pre-commit will format your code automatically on commit, but you should review the modified files before re-staging them.
- Write tests for new functionality and make sure they pass:
  ```bash
  python build.py --test
  cd build && ctest
  ```
- Open a Pull Request to the `develop` branch. Describe what the change does and why.

## Commit messages

Use conventional commits:

- `feat:` new feature
- `fix:` bug fix
- `refactor:` code restructuring without behaviour change
- `test:` adding or updating tests
- `docs:` documentation
- `build:` build system / CI
- `chore:` maintenance

Example: `fix(WallpaperManager): handle empty keyword list gracefully`

## What to work on

- Check issues labeled `good first issue` and `help wanted`.
- Add maintain for new DE.
- Add more services for fetching wallpaper.
- Add more functional in UI, example make configurations of config parameters in Settings menu.

## Need help?

Open a [discussion](https://github.com/Aloncie/Rwal/discussions) or comment on your Pull Request.
