#2. Stampa "file", "cartella" o "?" a seconda che il valore rappresenti un file (esi-
#stente), una cartella (esistente) o una voce non presente nel file-system

([[ -f $DATA ]] && [[ -e $DATA ]] && echo file ) || ([[ -d $DATA ]] && [[ -e $DATA ]] && echo cartella) || echo ?