import subprocess
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator


p = subprocess.run(['python', 'run_experiments.py', '1', 'conf.dat'], capture_output=True, text=True, check=True)
output = p.stdout.split()

times = []
for word in output:
    try:
        times.append(float(word))
    except ValueError:
        pass

fig, ax = plt.subplots()
ax.plot(list(range(1, len(times)+1)), times)

ax.set_ylim(ymin=0)
ax.ticklabel_format(useOffset=False, style='plain')
ax.xaxis.set_major_locator(MaxNLocator(integer=True))
ax.set(xlabel='Number of threads', ylabel='Time (microseconds)',
       title='Dependence of the runtime of the task on the number of threads')

fig.savefig("graph.png")
plt.show()
