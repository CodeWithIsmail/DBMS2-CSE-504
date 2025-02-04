import math
import random

# Load the Iris


def load_iris_data():
    data = []
    with open('iris.data', 'r') as file:
        for line in file:
            if line.strip():
                parts = line.strip().split(',')
                data.append((list(map(float, parts[:-1])), parts[-1]))
    return data

# Calculate entropy


def calculate_entropy(data):
    labels = [row[1] for row in data]

    # Manually count occurrences of each label
    label_counts = {}
    for label in labels:
        if label not in label_counts:
            label_counts[label] = 1
        else:
            label_counts[label] += 1

    total_samples = len(data)  # Total number of samples in the dataset
    entropy = 0.0
    for count in label_counts.values():
        probability = count / total_samples  # Probability of each label
        # Calculate entropy using the formula
        entropy -= probability * math.log2(probability)
    return entropy

# Split data based on an attribute and threshold


def split_data(data, attribute_index, threshold):
    left = [row for row in data if row[0][attribute_index] <= threshold]
    right = [row for row in data if row[0][attribute_index] > threshold]
    return left, right

# Calculate information gain


def calculate_information_gain(data, attribute_index, threshold):
    parent_entropy = calculate_entropy(data)
    left, right = split_data(data, attribute_index, threshold)
    left_weight = len(left) / len(data)
    right_weight = len(right) / len(data)
    weighted_entropy = left_weight * \
        calculate_entropy(left) + right_weight * calculate_entropy(right)
    return parent_entropy - weighted_entropy

# Find the best split


def find_best_split(data):
    best_gain = 0
    best_attribute = None
    best_threshold = None
    n_features = len(data[0][0])

    for attribute_index in range(n_features):
        values = set(row[0][attribute_index] for row in data)
        for threshold in values:
            gain = calculate_information_gain(data, attribute_index, threshold)
            if gain > best_gain:
                best_gain = gain
                best_attribute = attribute_index
                best_threshold = threshold

    return best_attribute, best_threshold, best_gain

# Create a decision tree


class DecisionTreeNode:
    def __init__(self, attribute_index=None, threshold=None, label=None, left=None, right=None):
        self.attribute_index = attribute_index
        self.threshold = threshold
        self.label = label
        self.left = left
        self.right = right


def build_decision_tree(data, depth=0):
    labels = [row[1] for row in data]

    # Check if all labels are the same (pure node)
    if len(set(labels)) == 1:  # Pure node, all labels are the same
        return DecisionTreeNode(label=labels[0])

    if not data:  # Empty dataset
        return None

    best_attribute, best_threshold, best_gain = find_best_split(data)

    # If no information gain, return majority label
    if best_gain == 0:
        # Manually count occurrences of each label
        label_counts = {}
        for label in labels:
            if label not in label_counts:
                label_counts[label] = 1
            else:
                label_counts[label] += 1

        # Find the label with the maximum count
        most_common_label = max(label_counts, key=label_counts.get)
        return DecisionTreeNode(label=most_common_label)

    left_data, right_data = split_data(data, best_attribute, best_threshold)
    left_subtree = build_decision_tree(left_data, depth + 1)
    right_subtree = build_decision_tree(right_data, depth + 1)

    return DecisionTreeNode(attribute_index=best_attribute, threshold=best_threshold, left=left_subtree, right=right_subtree)

# Make predictions


def predict(tree, sample):
    if tree.label is not None:
        return tree.label
    attribute_value = sample[tree.attribute_index]
    if attribute_value <= tree.threshold:
        return predict(tree.left, sample)
    else:
        return predict(tree.right, sample)

# Evaluate the decision tree


def evaluate(tree, test_data):
    correct = 0
    for sample, label in test_data:
        prediction = predict(tree, sample)
        if prediction == label:
            correct += 1
    accuracy = correct / len(test_data)
    return accuracy

# Print the decision tree


def print_tree(node, depth=0):
    if node is None:
        return

    # Print the label if it's a leaf node
    if node.label is not None:
        print("  " * depth + f"Leaf: {node.label}")
    else:
        # Print the decision rule at the current node
        print("  " * depth +
              f"Attribute {node.attribute_index} <= {node.threshold}?")

        # Recursively print the left and right subtrees
        print("  " * depth + "Left:")
        print_tree(node.left, depth + 1)

        print("  " * depth + "Right:")
        print_tree(node.right, depth + 1)


# Main function for cross-validation
if __name__ == "__main__":
    # Load dataset
    data = load_iris_data()

    accuracies = []
    num_iterations = 1

   # print(data)

    for _ in range(num_iterations):
        # Shuffle the dataset
        random.shuffle(data)

        # Split into training (120) and testing (30)
        train_data = data[:120]
        test_data = data[120:150]

        # Build decision tree
        decision_tree = build_decision_tree(train_data)

        # Print the decision tree structure (optional)
        print("Decision Tree Structure:")
        print_tree(decision_tree)

        # Evaluate decision tree
        accuracy = evaluate(decision_tree, test_data)
        print(f"Accuracy for iteration: {accuracy:.2f}")
        accuracies.append(accuracy)

    # Calculate and print the average accuracy
    average_accuracy = sum(accuracies) / num_iterations
    print(
        f"Average accuracy after {num_iterations} iterations: {average_accuracy:.2f}")
