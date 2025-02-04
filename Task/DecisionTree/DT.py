import math
import random
import numpy as np
from statistics import mean
from sklearn.model_selection import KFold
from sklearn.metrics import precision_score, recall_score, f1_score

label_map = {
    'Iris-setosa': 0,
    'Iris-versicolor': 1,
    'Iris-virginica': 2
}
label_map_index = {
    0: 'Iris-setosa',
    1: 'Iris-versicolor',
    2: 'Iris-virginica'
}
attribute_map = {
    0: 'sepal_length',
    1: 'sepal_width',
    2: 'petal_length',
    3: 'petal_width'
}


class DecisionTreeNode:
    def __init__(self, attribute_index=None, threshold=None, label=None, left=None, right=None):
        self.attribute_index = attribute_index
        self.threshold = threshold
        self.label = label
        self.left = left
        self.right = right


def load_data():
    data = []
    with open('iris.data', 'r') as file:
        for line in file:
            if line.strip():
                parts = line.strip().split(',')
                features = [float(x) for x in parts[:-1]]
                label = label_map[parts[-1]]
                data.append((features, label))
    return data


def split_data(data, attribute_index, threshold):
    left = []
    right = []
    for row in data:
        value = row[0][attribute_index]
        if value <= threshold:
            left.append(row)
        else:
            right.append(row)
    return left, right


def calculate_entropy(data):
    labels = [row[1] for row in data]
    label_count = {}

    for label in labels:
        label_count[label] = label_count.get(label, 0) + 1

    total_sample = len(data)
    entropy = sum(- (count / total_sample) * math.log2(count / total_sample)
                  for count in label_count.values())
    return entropy


def calculate_information_gain(data, attribute_index, threshold):
    left, right = split_data(data, attribute_index, threshold)
    parent_entropy = calculate_entropy(data)
    left_w = len(left) / len(data)
    right_w = len(right) / len(data)
    weighted_entropy = left_w * \
        calculate_entropy(left) + right_w * calculate_entropy(right)
    return parent_entropy - weighted_entropy


def calculate_chi_square(data, attribute_index, threshold):
    left, right = split_data(data, attribute_index, threshold)

    total_size = len(data)
    if total_size == 0 or len(left) == 0 or len(right) == 0:
        return 0  # Avoid division by zero or empty splits

    observed_left = [0] * 3  # Three classes: Setosa, Versicolor, Virginica
    observed_right = [0] * 3

    for row in left:
        observed_left[row[1]] += 1
    for row in right:
        observed_right[row[1]] += 1

    expected_left = [(len(left) * (observed_left[i] +
                      observed_right[i])) / total_size for i in range(3)]
    expected_right = [(len(right) * (observed_left[i] +
                       observed_right[i])) / total_size for i in range(3)]

    chi_square = 0
    for i in range(3):
        if expected_left[i] > 0:
            chi_square += ((observed_left[i] -
                           expected_left[i]) ** 2) / expected_left[i]
        if expected_right[i] > 0:
            chi_square += ((observed_right[i] -
                           expected_right[i]) ** 2) / expected_right[i]

    return chi_square


def find_best_split(data, criterion="entropy"):
    best_score = -float('inf')
    best_attribute = None
    best_threshold = None
    n_features = len(data[0][0])

    for attribute_index in range(n_features):
        values = set(row[0][attribute_index] for row in data)
        for threshold in values:
            if criterion == "entropy":
                score = calculate_information_gain(
                    data, attribute_index, threshold)
            elif criterion == "chi":
                score = calculate_chi_square(data, attribute_index, threshold)
            else:
                raise ValueError("Criterion must be 'entropy' or 'chi'")

            if score > best_score:
                best_score = score
                best_attribute = attribute_index
                best_threshold = threshold

    return best_attribute, best_score, best_threshold


def build_decision_tree(data, criterion="entropy", depth=0, max_depth=5):
    labels = [row[1] for row in data]

    if len(set(labels)) == 1:
        return DecisionTreeNode(label=labels[0])

    if not data:
        return None

    if max_depth is not None and depth >= max_depth:
        most_common_label = max(set(labels), key=labels.count)
        return DecisionTreeNode(label=most_common_label)

    best_attribute, best_score, best_threshold = find_best_split(
        data, criterion)

    if best_score == 0:
        most_common_label = max(set(labels), key=labels.count)
        return DecisionTreeNode(label=most_common_label)

    left_data, right_data = split_data(data, best_attribute, best_threshold)
    left_subtree = build_decision_tree(left_data, criterion, depth + 1)
    right_subtree = build_decision_tree(right_data, criterion, depth + 1)

    return DecisionTreeNode(attribute_index=best_attribute, threshold=best_threshold, left=left_subtree, right=right_subtree)


def predict(tree, sample):
    if tree.label is not None:
        return tree.label
    attribute_value = sample[tree.attribute_index]
    if attribute_value <= tree.threshold:
        return predict(tree.left, sample)
    else:
        return predict(tree.right, sample)


def f2_score(precision, recall):
    if precision + recall == 0:
        return 0
    return (5 * precision * recall) / (4 * precision + recall)


def evaluate_model(tree, test_data):
    true_labels = [label for _, label in test_data]
    predicted_labels = [predict(tree, sample) for sample, _ in test_data]

    precision = precision_score(true_labels, predicted_labels, average="macro")
    recall = recall_score(true_labels, predicted_labels, average="macro")
    f1 = f1_score(true_labels, predicted_labels, average="macro")
    f2 = f2_score(precision, recall)

    return precision, recall, f1, f2


def cross_validate(data, k, criterion="entropy"):
    kf = KFold(n_splits=k, shuffle=True, random_state=42)
    f1_scores = []
    f2_scores = []
    d2h_scores = []

    for train_index, test_index in kf.split(data):
        train_data = [data[i] for i in train_index]
        test_data = [data[i] for i in test_index]

        decision_tree = build_decision_tree(train_data, criterion, max_depth=5)

        precision, recall, f1, f2 = evaluate_model(decision_tree, test_data)
        print(
            f"precision: {precision*100:.2f}%, recall: {recall*100:.2f}%, f1: {f1*100:.2f}%, f2: {f2*100:.2f}%")
        f1_scores.append(f1)
        f2_scores.append(f2)

        d2h = np.sqrt((1 - precision)**2 + (1 - recall)**2)
        d2h_scores.append(d2h)

    print(f"Average F1 score: {sum(f1_scores) / len(f1_scores) * 100:.2f}%")
    print(f"Average F2 score: {sum(f2_scores) / len(f2_scores) * 100:.2f}%")
    print(f"Average d2h score: {round(mean(d2h_scores),3)}")


def main():
    dataset = load_data()
    criterion = input("Enter the criterion (entropy or chi): ")
    random.shuffle(dataset)
    cross_validate(dataset, 5, criterion=criterion)


if __name__ == "__main__":
    main()
