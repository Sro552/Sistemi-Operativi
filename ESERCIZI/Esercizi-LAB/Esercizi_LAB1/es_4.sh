#Scrivere uno script che dato un qualunque numero di argomenti li restituisca in
#output in ordine inverso.

lista=()

while [[ $1 != "" ]]; do
    lista[${#lista[@]}]=$1  #lista+=($1)
    shift
done

#for i in ${!lista[@]}; do
#	echo ${lista[$nargs-$i-1]}
#done

for i in ${!lista[@]}; do
    echo ${lista[${#lista[@]}-$i-1]}
done