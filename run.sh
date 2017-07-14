#!/bin/bash

for d in $(find * -name 'T*.TXT') 
do
  echo
  echo Processing $d with capacity 3.
  ./trabalho 3 $d >> results_3.txt

  echo
  echo Processing $d with capacity 5.
  ./trabalho 5 $d >> results_5.txt

  echo
  echo Processing $d with capacity 10.
  ./trabalho 10 $d >> results_10.txt
done

echo Done.

