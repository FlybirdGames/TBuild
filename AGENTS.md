# Agent instructions

Follow `docs/contributing/agent-guide.md` and the staged task files in `docs/agent-tasks/`.

Hard rules:

- Do not compile.
- Do not run tests.
- Do not change external behavior unless a task explicitly says so.
- Preserve CLI commands and options.
- Preserve DSL fields.
- Preserve lockfile format.
- Preserve artifact ID generation.
- Preserve cache layout.
- Keep Toolkit Package Manager scoped as a dependency manager for CMake projects.
