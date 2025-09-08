$$
\text{[Prog]} \to 
\begin{cases}
    \text{[Stmts]}
\end{cases} \\
\text{[Stmt]} \to 
\begin{cases}
    \text{exit([Expr]);} \\
    \text{var ident = ([Expr]);} \\
    \text{ident = [Expr];}
\end{cases} \\
\text{[Expr]} \to 
\begin{cases}
    \text{[Term]} \\
    \text{[BinExpr]}
\end{cases} \\
\text{[BinExpr]} \to 
\begin{cases}
    \text{[Expr] + [Expr]} & \text{pred = 0} \\
    \text{[Expr] * [Expr]} & \text{pred = 1}
\end{cases} \\
\text{[Term]} \to
\begin{cases}
    \text{int\_let} \\
    \text{ident} \\
\end{cases}

$$