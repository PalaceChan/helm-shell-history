# helm-shell-history.el

## Introduction
`helm-shell-history` is a helm frontend to shell history and currently supports bash history.

It works by querying `history` via shell command and presenting completion candidates in recency order.
The sole completion action is to insert the selected command at the end of the buffer (for optionally editing and easily running the command again in the shell emulator)

![Helm Shell GIF](media/preview.gif?raw=true)

## Requirements

* Emacs 25.3 or higher
* Helm 1.9.9 or higher

## Example Installation

```bash
mkdir -p ~/.emacs.d/lisp/helm-shell-history
git clone https://github.com/PalaceChan/helm-shell-history.git ~/.emacs.d/lisp/helm-shell-history
```

## Example Configuration

```lisp
  (use-package helm-shell-history
    :load-path "~/.emacs.d/lisp/helm-shell-history"
    :after term
    :config    
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
