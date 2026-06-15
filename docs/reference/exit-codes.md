# Exit codes

ToolkitBuild currently uses a simple exit-code policy.

| Code | Meaning |
| --- | --- |
| `0` | Success |
| non-zero | Failure |

For diagnostics, prefer reading command output and structured diagnostic output when available.
