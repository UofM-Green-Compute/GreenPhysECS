set datafile separator ","
set terminal epslatex size 8.5cm, 7.5cm
set output "SIS.tex"
set size square

set xlabel "Time (days)"
set ylabel "No. People"

set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#D55E00" lw 2 dt 1

set xtics ("0" 0, "$1$" 1, "$2$" 2, "$3$" 3, "$4$" 4, "$5$" 5)
set ytics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set xrange [0:5]
set yrange [0:100]
set xtics in
set ytics in

set border back
set pointsize 2

set arrow 1 from graph 0, first 14 to graph 1, first 14 \
    nohead dt 2 lw 2 lc rgb "black"

set arrow 2 from graph 0, first 86 to graph 1, first 86 \
    nohead dt 2 lw 2 lc rgb "black"

set label 1 "$14$" at 0.1,14 front
set label 2 "$86$" at 0.1,86 front

plot \
    "SIS_1.txt" using 1:2 with lines ls 1 notitle, \
    "SIS_1.txt" using 1:3 with lines ls 2 notitle
