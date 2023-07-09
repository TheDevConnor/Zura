import time

# Function to benchmark


def my_function():
    # Perform some operations here
    time.sleep(1)  # Simulating some time-consuming operation
    return


# Perform benchmarking
benchmark_results = []
num_iterations = 5  # Number of iterations to perform

for i in range(num_iterations):
    start_time = time.time()

    # Call the function you want to benchmark
    my_function()

    end_time = time.time()
    execution_time = end_time - start_time

    # Store the results
    benchmark_results.append(
        {'Iteration': i + 1, 'Execution Time': execution_time})

# Display the results in a tabular format
header = "| Iteration | Execution Time (s) |"
separator = "+------------+-------------------+"

print(separator)
print(header)
print(separator)

for result in benchmark_results:
    iteration = result['Iteration']
    execution_time = result['Execution Time']
    row = f"| {iteration:^10} | {execution_time:^17.6f} |"
    print(row)

print(separator)
