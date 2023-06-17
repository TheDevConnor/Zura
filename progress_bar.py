import sys
import time
import subprocess
from threading import Thread

def print_progress_bar(iteration, total, prefix='', suffix='', decimals=1, length=50, fill='█'):
    percent = ('{0:.' + str(decimals) + 'f}').format(100 * (iteration / float(total)))
    filled_length = int(length * iteration // total)
    bar = fill * filled_length + '-' * (length - filled_length)
    sys.stdout.write('\r%s |%s| %s%% %s' % (prefix, bar, percent, suffix))
    sys.stdout.flush()

def run_build_command(command):
    subprocess.call(command, shell=True)

# Run the build command
build_command = ' '.join(sys.argv[1:])  # Get build command from command line arguments
print("Building with command: " + build_command)

# Start a separate thread to run the build command
build_thread = Thread(target=run_build_command, args=(build_command,))
build_thread.start()

# Keep updating the progress bar until the build thread completes
progress = 0
while build_thread.is_alive():
    progress += 1
    print_progress_bar(progress, 60, prefix='Progress:', suffix='Complete', length=50)
    time.sleep(1)

# Wait for the build thread to complete
build_thread.join()

# Print final progress bar at 100%
print_progress_bar(60, 60, prefix='Progress:', suffix='Complete', length=50)
print("\nBuild complete!")
