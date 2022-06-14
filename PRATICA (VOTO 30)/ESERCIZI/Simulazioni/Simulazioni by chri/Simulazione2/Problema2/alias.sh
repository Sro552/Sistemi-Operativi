#!/bin/bash

data="";
longest="";

while [[ $data != "quit" ]]; do
	echo -n "Inserire parola: ";
	read data;

	if [[ $data != "quit" ]]; then
		if [[ ${#data} > ${#longest} ]]; then
			longest=$data;
		fi
	fi
done

>&1 echo ${#longest};
>&2 echo ${longest};
