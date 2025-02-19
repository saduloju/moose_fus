set terminal postscript
set output 'all.ps'
set title 'All Postprocessors'
set xlabel 'time'
set ylabel 'values'
plot 'output_test_gnuplot_ps_out.dat' using 1:2 title 'dofs' with linespoints

set output 'dofs.ps'
set ylabel 'dofs'
plot 'output_test_gnuplot_ps_out.dat' using 1:2 title 'dofs' with linespoints

