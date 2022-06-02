#5. Scrivere uno script che mostri il contenuto della cartella corrente in ordine


lista=($(ls))
for i in ${!lista[@]}; do
	echo ${lista[${#lista[@]}-$i-1]}
done

