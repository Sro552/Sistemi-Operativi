#. Stampa âTâ (per True) o âFâ (per False) a seconda che il valore rappresenti un
#file o cartella esistente

[[ -f $DATA ]] && echo T; [[ -d $DATA ]] && echo F;