#!/bin/bash
#
# Carlos Contreras Bolton
# Script para ejecutar AG-DTP
# Se puede ejecutar con o sin semilla aleatorea
#

IN=$1
N=$2
SEED=$3

if [ "$IN" = "" ] && [ "$SEED" = "" ]
then
    echo "usage:"
    echo "      ./ejecutar arg1 arg2"
    echo ""
    echo "arg1 = Archivo de entrada"
    echo "arg2 = Si no se desea semilla aleatoria, colocar valor"
    exit
fi

for j in `seq 1 $N`;
do
CONTADOR=0
for linea in $(cat $IN)
do
    let CONTADOR=CONTADOR+1
    # if [ $CONTADOR = 1 ]
    # then
    # 	instancia="$linea"
    # fi
done;

if [ "$SEED" = "" ]
then
    sed -i "s/$linea/$((RANDOM%1000000))/" $IN
else
    sed -i "s/$linea/$SEED/" $IN
fi

./gctsp $IN
done;
# FECHA=`date +%d_%m_%Y_%H_%M_%S`
# instancia=${instancia%%.txt}
# mkdir salida/$instancia"_"$FECHA
