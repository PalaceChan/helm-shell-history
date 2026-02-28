# AGENTS.md — helm-shell-history

## Commands
- Build fast parser (C): `make -C src`  # produces `src/parse_history`
- Run all parser tests: `make -C src test`
- Run one parser test: `make -C src t0` (also: `t1`, `t2`)
- Debug a single test: `src/test/run.sh ./src/parse_history t0 '%Y%m%d %T' 9999`
- Clean build artifacts: `make -C src clean`
- “Lint” (stricter warnings): `make -C src CFLAGS='-Wall -Wextra -O2 -g'`
- Optional elisp sanity: `emacs -Q --batch -L elisp -f batch-byte-compile elisp/helm-shell-history.el`

## Code style (match existing code)
- Emacs Lisp (`elisp/`): 2-space indent; keep `lexical-binding: t`; prefix public APIs with `helm-shell-history-`.
- Keep `require` statements near the top and `provide` at the end; add any new dependency via an explicit `require`.
- Use `defcustom`/`defgroup` for user-facing options and include docstrings; keep customization vars in the same `:group`.
- C (`src/`): keep includes minimal/ordered; prefer `size_t` for sizes and `const` for read-only pointers.
- Error handling: always check syscalls/allocations; use existing `die()`/`usage()` helpers and fail loudly.
- Formatting: keep indentation/braces consistent within the touched file; avoid drive-by reformatting.
