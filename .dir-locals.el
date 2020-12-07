((nil . ((eval . (add-to-list 'auto-mode-alist '("\\.h\\'" . c++-mode)))
         (eval . (add-to-list 'auto-mode-alist '("\\.uml" . python-mode)))))
 (python-mode . ((fill-column . 79)))
 (c++-mode . ((c-basic-offset . 4)
              (fill-column . 90)
              (eval . (progn
                        (defun do-shell (s)
                          ;; Helper for running a shell command and getting the first line
                          ;; of its output.
                          (substring (shell-command-to-string s) 0 -1))

                        (setq flycheck-gcc-include-path
                              (list
                               (concat
                                (do-shell "git rev-parse --show-toplevel")
                                "/utils/include")))))
              (flycheck-gcc-language-standard . "gnu++17")))))
