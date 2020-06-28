# helm-shell-history.el

## Introduction
`helm-shell-history` is a helm frontend to bash history.

It comes with a fast c parser which you can compile by just running `make` (`make test` runs its tests).
You can configure it to use the fast parser by setting `helm-shell-history-fast-parser` to point to the binary.
When the fast binary is not configured, it falls back on using the `history` shell built-in to get the candidates.

Candidates are presented in recency order.
The completion action is to insert the selected command at the end of the buffer

![Helm Shell GIF](media/preview2.gif?raw=true)

## Requirements

* Emacs 25.3 or higher
* Helm 1.9.9 or higher

## Example Installation

```bash
mkdir -p ~/.emacs.d/lisp/helm-shell-history
git clone https://github.com/PalaceChan/helm-shell-history.git ~/.emacs.d/lisp/helm-shell-history

#to unlock much faster parsing of your history
(cd ~/.emacs.d/lisp/helm-shell-history/src && make)
```

## Example Configuration

A minimal configuration:

```lisp
  (use-package helm-shell-history
    :load-path "~/.emacs.d/lisp/helm-shell-history/elisp"
    :after term
    :config
    (define-key term-mode-map (kbd "M-r") 'helm-shell-history))
```

A much faster (recommended) configuration:

```lisp
  (use-package helm-shell-history
    :load-path "~/.emacs.d/lisp/helm-shell-history/elisp"
    :after term
    :config
    (setq helm-shell-history-fast-parser "~/.emacs.d/lisp/helm-shell-history/src/parse_history")
    (define-key term-mode-map (kbd "M-r") 'helm-shell-history))
```

## Customize Variables

#### `helm-shell-history-file` (Default `~/.bash_history`)

History file to use

#### `helm-shell-history-time-format` (Default `%Y%m%d %T`)

Date/timestamp prefix to use when displaying candidates.
If you override this in a way that alters the count of displayed prefix tokens make sure to also update `helm-shell-history-prefix-tokens`

#### `helm-shell-history-prefix-tokens` (Default `3`)

Tokens to skip when inserting selected command (the default of 3 corresponds to the default command number, date, and time)

#### `helm-shell-history-fuzzy-match` (Default `nil`)

Set to `t` if you want fuzzy matching of candidates.

#### `helm-shell-history-candidate-limit` (Default `99999`)

Limit the number of candidates to this number of most recent shell commands.

#### `helm-shell-history-fast-parser` (Default "")

Point to the compiled binary for the fast parsing to unlock much faster parsing