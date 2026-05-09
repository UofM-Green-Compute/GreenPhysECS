set datafile separator ","
set terminal epslatex size 8.5cm, 7.5cm
set output "SEIR.tex"
set size square

set xlabel "Time (days)"
set ylabel "No. People"

set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#009E73" lw 2 dt 1
set style line 3 lc rgb "#D55E00" lw 2 dt 1
set style line 4 lc rgb "#00000" lw 2 dt 1

set xtics ("0" 0, "$2$" 2, "$4$" 4, "$6$" 6, "$8$" 8, "$10$" 10)
set ytics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set xrange [0:10]
set yrange [0:100]
set xtics in
set ytics in

set border back
set pointsize 2

plot \
    "SEIR_1.txt"   using 1:2 with lines ls 1 notitle, \
    "SEIR_1.txt"   using 1:3 with lines ls 2 notitle, \
    "SEIR_1.txt"   using 1:4 with lines ls 3 notitle, \
    "SEIR_1.txt"   using 1:5 with lines ls 4 notitle, \
