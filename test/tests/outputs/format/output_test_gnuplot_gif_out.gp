set terminal gif
set output 'all.gif'
set title 'All Postprocessors'
set xlabel 'time'
set ylabel 'values'
plot 'output_test_gnuplot_gif_out.dat' using 1:2 title 'dofs' with linespoints

set output 'dofs.gif'
set ylabel 'dofs'
plot 'output_test_gnuplot_gif_out.dat' using 1:2 title 'dofs' with linespoints

