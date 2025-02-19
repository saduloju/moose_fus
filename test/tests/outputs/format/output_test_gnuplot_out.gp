set terminal png
set output 'all.png'
set title 'All Postprocessors'
set xlabel 'time'
set ylabel 'values'
plot 'output_test_gnuplot_out.dat' using 1:2 title 'dofs' with linespoints

set output 'dofs.png'
set ylabel 'dofs'
plot 'output_test_gnuplot_out.dat' using 1:2 title 'dofs' with linespoints

