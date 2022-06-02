#4. Scrivere uno script che dato un qualunque numero di argomenti li restituisca in
#output in ordine inverso

nargs=$#
lista=()

while [[ $1 != "" ]]; do
	lista+=($1)
	shift
done

for i in ${!lista[@]}; do
	echo ${lista[$nargs-$i-1]}
done