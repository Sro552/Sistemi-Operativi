# 3. Stampa il risultato di una semplice operazione aritmetica (es: ‘1 < 2’) contenuta
# nel file indicato dal valore di DATA, oppure “?” se il file non esiste

([[ ! -e  $DATA ]] && echo ? ) || echo $(( $(cat $DATA)))