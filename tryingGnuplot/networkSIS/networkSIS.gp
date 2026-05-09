set datafile separator ","
set terminal epslatex size 8.5cm, 7.5cm
set output "networkSIS.tex"
set size square

set xlabel "Time (days)"
set ylabel "No. People"

set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#D55E00" lw 2 dt 1

set xtics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set ytics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set xrange [0:100]
set yrange [0:100]
set xtics in
set ytics in

set border back
set pointsize 2

plot \
    "Network-SIS.txt"   using 1:2 with lines ls 1 notitle, \
    "Network-SIS.txt"   using 1:3 with lines ls 2 notitle, \
