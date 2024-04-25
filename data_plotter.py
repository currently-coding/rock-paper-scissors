import matplotlib.pyplot as plt
import numpy as np
from collections import Counter, defaultdict

# Input data in the specified format
input_data = [
    (1, 2, 1), (1, 1, 0), (1, 3, -1), (2, 2, 0), (1, 1, 0), (1, 1, 0), (3, 1, 1), (2, 3, 1), (3, 1, 1), (1, 2, 1), (3, 2, -1), (2, 1, -1), (1, 1, 0), (1, 3, -1), (1, 3, -1), (3, 2, -1), (3, 1, 1), (1, 2, 1), (3, 1, 1), (1, 2, 1),
]

# Separate data into x (computer move), y (human next move), and results
x = [entry[0] for entry in input_data]
y = [entry[1] for entry in input_data]
results = [entry[2] for entry in input_data]

# Get unique results
unique_results = set(results)

# Create dictionaries to store mappings
loss_map = defaultdict(list)
win_map = defaultdict(list)
draw_map = defaultdict(list)

# Create subplots for each result
for i, result in enumerate(unique_results):
    # Filter data based on result
    x_subset = [x[j] for j in range(len(results)) if results[j] == result]
    y_subset = [y[j] for j in range(len(results)) if results[j] == result]

    # Count the occurrences of each pair of moves in the subset
    move_counts = Counter(zip(x_subset, y_subset))

    # Populate mappings
    for move_pair, count in move_counts.items():
        probability = count / len(x_subset)
        if result == -1:
            loss_map[move_pair[0]].append((move_pair[1], probability))
        elif result == 1:
            win_map[move_pair[0]].append((move_pair[1], probability))
        else:
            draw_map[move_pair[0]].append((move_pair[1], probability))

# Print mappings
def format_mapping(mapping):
    formatted = []
    for computer_move, moves in mapping.items():
        probabilities = [f"({move}, {prob:.2f})" for move, prob in moves]
        formatted.append(f"{computer_move}: {', '.join(probabilities)}")
    return formatted

print("Mapping for Result -1 (Loss):")
print('\n'.join(format_mapping(loss_map)))
print("\nMapping for Result 1 (Win):")
print('\n'.join(format_mapping(win_map)))
print("\nMapping for Result 0 (Draw):")
print('\n'.join(format_mapping(draw_map)))