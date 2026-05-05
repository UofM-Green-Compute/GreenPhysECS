set terminal epslatex size 17cm, 1.5cm
set output "SIRlegend.tex"

unset border
unset tics
unset xlabel
unset ylabel
set yrange [0:1]
set xrange [0:1]

set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#D55E00" lw 2 dt (6,6)
set style line 3 lc rgb "#00000" lw 2 dt (2,6)

set key center center horizontal box
set key spacing 2

plot \
    NaN with lines ls 1 title "Susceptible", \
    NaN with lines ls 2 title "Infected", \
    NaN with lines ls 3 title "Recovered"