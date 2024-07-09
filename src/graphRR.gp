# Set the output format and file name (e.g., PDF, PNG, or SVG)
set terminal jpeg
set output 'rrPerformance.jpeg'

# Set the title and labels for the x and y axes
set title "Round Robin Scheduler Performance"
set xlabel "Average Metrics"
set ylabel "Time"

set style fill solid
set boxwidth 0.3 relative

# Define the data file
datafile1 = "metricsRR.dat"
# datafile2 = "metricsRR.dat"

# Create the bar graph
plot datafile1 using 2:xtic(1) with boxes linecolor rgb "blue" title "Round Robin"
# replot datafile2 using 2:xtic(1) with boxes title "Round Robin"
