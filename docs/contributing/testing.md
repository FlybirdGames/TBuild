# Testing

## Scope

Existing tests should continue to validate model loading, lockfiles, dependency resolver behavior, package artifacts, CMake export, Lua DSL parsing, package builder helpers, toolchain handling, and artifact garbage collection.

## Refactor validation

For no-behavior-change refactors:

- Do not add unrelated behavior tests.
- Update includes and file paths as needed.
- Keep test expectations unchanged unless the task explicitly changes behavior.

## Owner-run validation

The project owner runs compilation and tests locally. Agent tasks should not compile or test unless explicitly told to do so.
