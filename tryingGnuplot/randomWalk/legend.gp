set terminal epslatex size 6.7cm, 1.5cm
set output "legend.tex"

unset border
unset tics
unset xlabel
unset ylabel
set yrange [0:1]
set xrange [0:1]

set style line 1 lc rgb "#56B4E9" lw 4 dt 1
set style line 2 lc rgb "#D55E00" lw 4 dt 1

set key center center horizontal
set key spacing 2

plot \
    NaN with lines ls 1 title "Walker 1", \
    NaN with lines ls 2 title "Walker 2"