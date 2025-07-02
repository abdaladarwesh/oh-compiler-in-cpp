$$
\text{Prog} \to 
\begin{cases}
    \text{Stmts}
\end{cases} \\
\text{stmt} \to 
\begin{cases}
    \text{exit([Expr]);} \\
    \text{var ident = ([Expr]);} \\
    \text{ident = [Expr];}
\end{cases} \\
\text{Expr} \to 
\begin{cases}
    \text{[Term]} \\
\end{cases} \\
\text{[Term]} \to
\begin{cases}
    \text{int\_let} \\
    \text{ident}
\end{cases}
$$