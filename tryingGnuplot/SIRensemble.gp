set datafile separator ","
set terminal epslatex size 10.5cm, 5.25cm
set output "SIRensemble.tex"

set xlabel "Time (t)"
set ylabel "No. People"

set style fill transparent solid 0.3 noborder
set style line 1 lc rgb "#007FFF" lw 2
set style line 2 lc rgb "#FF7F00" lw 2
set style line 3 lc rgb "#008000" lw 2

files = "SIRensembleSusceptibleBeta7I10.txt \
         SIRensembleInfectedBeta7I10.txt \
         SIRensembleRecoveredBeta7I10.txt" 

unset colorbox

set xtics ("0" 0, "$1$" 1, "$2$" 2, "$3$" 3, "$4$" 4, "$5$" 5)
set ytics ("0" 0, "$20$" 20, "$40$" 40, "$60$" 60, "$80$" 80, "$100$" 100)
set xrange [0:5]
set yrange [0:100]
set xtics in nomirror
set ytics in nomirror


plot \
    "SIRensembleSusceptibleBeta7I10.txt" using 1:3:4 with filledcurves ls 1 notitle, \
    "SIRensembleSusceptibleBeta7I10.txt" using 1:2 with lines ls 1 notitle, \
    "SIRensembleSusceptibleBeta7I10.txt" using 1:3 with lines ls 1 notitle, \
    "SIRensembleSusceptibleBeta7I10.txt" using 1:4 with lines ls 1 notitle, \

    "SIRensembleInfectedBeta7I10.txt" using 1:3:4 with filledcurves ls 2 notitle, \
    "SIRensembleInfectedBeta7I10.txt" using 1:2 with lines ls 2 notitle, \
    "SIRensembleInfectedBeta7I10.txt" using 1:3 with lines ls 2 notitle, \
    "SIRensembleInfectedBeta7I10.txt" using 1:4 with lines ls 2 notitle, \

    "SIRensembleRecoveredBeta7I10.txt" using 1:3:4 with filledcurves ls 3 notitle, \
    "SIRensembleRecoveredBeta7I10.txt" using 1:2 with lines ls 3 notitle, \
    "SIRensembleRecoveredBeta7I10.txt" using 1:3 with lines ls 3 notitle, \
    "SIRensembleRecoveredBeta7I10.txt" using 1:4 with lines ls 3 notitle, \
