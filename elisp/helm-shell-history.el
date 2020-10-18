;;; helm-shell-history.el --- Shell command history helm interface -*- lexical-binding: t -*-

;; Version: 0.2.0
;; Author: Palace Chan
;; URL: https://github.com/PalaceChan/helm-shell-history
;; Keywords: convenience, helm, shell, history
;; Package-Requires ((emacs "25.3") (helm "1.9.9"))

;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;; See the GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;; A helm frontend for searching shell command history

;;; Code:

(require 'seq)
(require 'helm)

;; Customization

(defgroup helm-shell-history nil
  "A helm frontend for searching shell command history"
  :group 'helm)

(defcustom helm-shell-history-candidate-limit 99999
  "Limit search this many most recent entries in your history."
  :type 'integer
  :group 'helm-shell-history)

(defcustom helm-shell-history-file "~/.bash_history"
  "Path to history file."
  :type 'string
  :group 'helm-shell-history-file)

(defcustom helm-shell-history-time-format "%Y%m%d %T"
  "Equivalent of HISTTIMEFORMAT.
 If you override this, you need to override helm-shell-history-prefix-tokens"
  :type 'string
  :group 'helm-shell-history-file)

(defcustom helm-shell-history-prefix-tokens 3
  "Set this to the number of extra tokens prepended by your history display format.
For example if a history line looks like:

 2653 20200609 18:00:00 echo hello

set it to 3 so as to skip '2653 20200609 18:00:00'"
  :type 'integer
  :group 'helm-shell-history-file)

(defcustom helm-shell-history-fuzzy-match nil
  "Whether to fuzzy match for helm completion."
  :type 'boolean
  :group 'helm-shell-history-file)

(defcustom helm-shell-history-fast-parser ""
  "Point to the location of the compiled fast parser"
  :type 'string
  :group 'helm-shell-history-file)

;; Implementation

(defvar helm-shell-history-buffer "*Helm Shell History*")

(defvar helm-shell-history-reverse-cmd
  (if (executable-find "tac")
      "tac"
    "awk '{a[i++]=$0} END {for (j=i-1; j>=0;) print a[j--] }'"))

(defun get-helm-shell-history-shell-cmd-and-sep ()
  (if (file-executable-p helm-shell-history-fast-parser)
      (cons (format "%s %s '%s' %s" helm-shell-history-fast-parser helm-shell-history-file
		    helm-shell-history-time-format helm-shell-history-candidate-limit) "\0")
    (cons
     (format "HISTFILE=%s; HISTTIMEFORMAT='%s '; history -r $HISTFILE; \
 history | tail -n %s | %s"
	     helm-shell-history-file helm-shell-history-time-format
	     helm-shell-history-candidate-limit helm-shell-history-reverse-cmd) "\n")))

(defun helm-shell-history-build-source ()
  (let* ((helm-shell-history-shell-cmd-and-sep (get-helm-shell-history-shell-cmd-and-sep))
	 (shell-cmd (car helm-shell-history-shell-cmd-and-sep))
	 (shell-cmd-sep (cdr helm-shell-history-shell-cmd-and-sep)))
    (seq-remove #'string-blank-p 
		(split-string (shell-command-to-string shell-cmd) shell-cmd-sep))))

(defun helm-shell-history-term-insert (arg)
  (insert (s-prepend " " arg)))

(defun helm-shell-history-vterm-insert (arg)
  (let ((inhibit-read-only t))
    (vterm-send-string arg t)))

(defun helm-shell-history-command-action (arg)
  "Insert command portion of ARG in current buffer"  
  (let ((cmd (string-join
	      (nthcdr helm-shell-history-prefix-tokens 
		      (split-string arg)) " ")))
    (goto-char (point-max))
    (skip-chars-backward "\n[:space:]")
    (forward-char)
    (if (equal major-mode 'vterm-mode)
	(helm-shell-history-vterm-insert cmd)
      (helm-shell-history-term-insert cmd))))

;;;###autoload
(defun helm-shell-history ()
  "Find command in shell history."
  (interactive)
  (helm :sources (helm-build-sync-source "Shell history"
		   :candidates 'helm-shell-history-build-source
		   :multiline t
		   :fuzzy-match helm-shell-history-fuzzy-match
		   :action 'helm-shell-history-command-action)
	:buffer helm-shell-history-buffer
	:prompt "Find command: "))

(provide 'helm-shell-history)

;;; helm-shell-history.el ends here
