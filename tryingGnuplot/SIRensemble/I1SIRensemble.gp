set datafile separator ","
set terminal epslatex size 8.5cm, 7.5cm
set output "I1SIRensemble.tex"
set size square

set xlabel "Time (days)"
set ylabel "No. People"

set style fill transparent solid 0.3 noborder
set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#D55E00" lw 2 dt (6,6)
set style line 3 lc rgb "#00000" lw 2 dt (2,6)

set style line 4 lc rgb "#D55E00" lw 2 dt 1
set style line 5 lc rgb "#00000" lw 2 dt 1

set xtics ("0" 0, "$1$" 1, "$2$" 2, "$3$" 3, "$4$" 4, "$5$" 5)
set ytics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set xrange [0:5]
set yrange [0:100]
set xtics in
set ytics in

set border back
set pointsize 2

plot \
    "SIRensembleRecoveredBeta7I1.txt"   using 1:3:4 with filledcurves ls 3 notitle, \
    "SIRensembleInfectedBeta7I1.txt"    using 1:3:4 with filledcurves ls 2 notitle, \
    "SIRensembleSusceptibleBeta7I1.txt" using 1:3:4 with filledcurves ls 1 notitle, \
    "SIRensembleRecoveredBeta7I1.txt"   using 1:2 with lines ls 3 notitle, \
    "SIRensembleRecoveredBeta7I1.txt"   using 1:3 with lines ls 5 notitle, \
    "SIRensembleRecoveredBeta7I1.txt"   using 1:4 with lines ls 5 notitle, \
    "SIRensembleInfectedBeta7I1.txt"    using 1:2 with lines ls 2 notitle, \
    "SIRensembleInfectedBeta7I1.txt"    using 1:3 with lines ls 4 notitle, \
    "SIRensembleInfectedBeta7I1.txt"    using 1:4 with lines ls 4 notitle, \
    "SIRensembleSusceptibleBeta7I1.txt" using 1:2 with lines ls 1 notitle, \
    "SIRensembleSusceptibleBeta7I1.txt" using 1:3 with lines ls 1 notitle, \
    "SIRensembleSusceptibleBeta7I1.txt" using 1:4 with lines ls 1 notitle, \
