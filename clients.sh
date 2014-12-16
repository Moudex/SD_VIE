#!/bin/bash

cmpt=0

while [ "$cmpt" != "$1" ]
do
    ./client quiet &
    cmpt=$(($cmpt+1))
done

exit 0
