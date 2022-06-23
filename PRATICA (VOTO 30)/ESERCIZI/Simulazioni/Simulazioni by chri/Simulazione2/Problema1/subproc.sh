#!/bin/bash

for n in $(seq 1 10); do
	echo $(echo $BASHPID)
done
