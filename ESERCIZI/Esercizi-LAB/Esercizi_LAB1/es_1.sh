#. Stampa “T” (per True) o “F” (per False) a seconda che il valore rappresenti un
#file o cartella esistente

[[ -f $DATA ]] && echo T; [[ -d $DATA ]] && echo F;