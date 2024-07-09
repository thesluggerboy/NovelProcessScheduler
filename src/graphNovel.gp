# Set the output format and file name (e.g., PDF, PNG, or SVG)
set terminal jpeg
set output 'novelPerformance.jpeg'

# Set the title and labels for the x and y axes
set title "Novel Scheduler Performance"
set xlabel "Average Metrics"
set ylabel "Time"

set style fill solid
set boxwidth 0.3 relative

# Define the data file
datafile1 = "metricsMLFQ.dat"
# datafile2 = "metricsRR.dat"

# Create the bar graph
plot datafile1 using 2:xtic(1) with boxes linecolor rgb "red" title "NovelScheduler"
# replot datafile2 using 2:xtic(1) with boxes title "Round Robin"
