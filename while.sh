#!/usr/bin/bash


i=1


# the number 4 can be the limit to


# iterate the loop


while [ $i -le 100 ];
do
    curl http://localhost:4243
    ((i++))
done