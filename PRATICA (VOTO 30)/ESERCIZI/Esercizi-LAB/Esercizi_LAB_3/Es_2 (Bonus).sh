# 2. creare un “alias” in bash denominato “somma” che legge un numero (intero con
# segno) alla volta (numero+INVIO, numero+INVIO, …) e alla fine (immissione vuota
# premendo solo INVIO) stampa la somma dei numeri inseriti.


alias somma='var="."; somma="0"; while [[ $var != "" ]]; do read var; somma="$somma+$var"; done; somma="$somma 0"; echo $(( $somma ));'