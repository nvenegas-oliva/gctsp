#!/usr/bin/gnuplot -persist
# This file is part of VCP-PGB
#
# Copyright (C) 2008 Carlos Contreras Bolton <ccontreras.bolton@gmail.com>
#
# VCP-PGB is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# VCP-PGB is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

#plot3Curves: [Generacion v/s Costo Ruta]...
reset #Reset previous configurations.
#clear #Clear the window
set terminal png enhanced size 800,400
set output "gen_vs_costo.png"
set grid
set key outside #Legend outside the graph limits
set offsets 0, 0, 0.001, 0 #Force an offset at top (helps when worse curve is straight)
set lmargin 9
set bmargin 3
set key right box
set title "Coste Total" font ",10"
set xlabel "Generación"
set ylabel "Costo Ruta"
plot 'generacion.csv' using 1:4 title "Mejor" with lines linetype 4 linewidth 2, 'generacion.csv' using 1:2 title "Promedio" with lines linetype 2 linewidth 2, 'generacion.csv' using 1:3 title "Peor" with lines linetype 3 linewidth 2

#plot3Curves: [Generacion v/s Costo Ruta]...
reset #Reset previous configurations.
clear #Clear the window
set terminal png enhanced size 800,400
set output "gen_vs_costoAvg.png"
set grid
set key outside #Legend outside the graph limits
set offsets 0, 0, 0.001, 0 #Force an offset at top (helps when worse curve is straight)
set lmargin 9
set bmargin 3
set key right box
set title "Coste Total (solo el mejor)" font ",10"
set xlabel "Generación"
set ylabel "Costo Ruta"
plot 'generacion.csv' using 1:4 title "Mejor" with lines linetype 4 linewidth 2

#Graficar tour
reset #Reset previous configurations.
clear #Clear the window
set terminal png enhanced size 800,400
set output "ruta.png"
plot "layout.txt" using 2:3 title "Ruta" with linespoints 
