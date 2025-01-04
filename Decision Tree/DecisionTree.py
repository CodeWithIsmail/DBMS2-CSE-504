import math
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
    def __init__(self, attribute_index=None, threshold=None, label=None,  left=None, right=None):
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
        if label not in label_count:
            label_count[label] = 1
        else:
            label_count[label] += 1
    total_sample = len(data)
    entropy = 0.0
    for count in label_count.values():
        probability = count/total_sample
        entropy -= probability*math.log2(probability)
    return entropy


def calculate_information_gain(data, attribute_index, threshold):
    left, right = split_data(data, attribute_index, threshold)
    left_entropy = calculate_entropy(left)
    right_entropy = calculate_entropy(right)
    parent_entropy = calculate_entropy(data)
    left_w = len(left)/len(data)
    right_w = len(right)/len(data)
    weighted_entropy = left_w*left_entropy + right_w*right_entropy
    information_gain = parent_entropy - weighted_entropy
    return information_gain


def calculate_gini_impurity(data):
    labels = [row[1] for row in data]
    label_count = {}

    for label in labels:
        if label not in label_count:
            label_count[label] = 1
        else:
            label_count[label] += 1

    total_samples = len(data)
    gini = 1.0
    for count in label_count.values():
        probability = count / total_samples
        gini -= probability ** 2
    return gini


def calculate_gini_gain(data, attribute_index, threshold):
    left, right = split_data(data, attribute_index, threshold)
    left_gini = calculate_gini_impurity(left)
    right_gini = calculate_gini_impurity(right)
    parent_gini = calculate_gini_impurity(data)
    left_w = len(left) / len(data)
    right_w = len(right) / len(data)
    weighted_gini = left_w * left_gini + right_w * right_gini
    gini_gain = parent_gini - weighted_gini
    return gini_gain


def find_best_split(data, criterion="entropy"):
    # Initialize with negative infinity to ensure any score is better.
    best_score = -float('inf')
    best_attribute = None
    best_threshold = None
    n_features = len(data[0][0])

    for attribute_index in range(n_features):
        values = set([row[0][attribute_index] for row in data])
        for threshold in values:
            if criterion == "entropy":
                score = calculate_information_gain(
                    data, attribute_index, threshold)
            elif criterion == "gini":
                score = calculate_gini_gain(data, attribute_index, threshold)
            else:
                raise ValueError("Criterion must be 'entropy' or 'gini'")

            if score > best_score:
                best_score = score
                best_attribute = attribute_index
                best_threshold = threshold
    return best_attribute, best_score, best_threshold


def build_decision_tree(data, criterion="entropy", depth=0):
    labels = [row[1] for row in data]

    if len(set(labels)) == 1:
        return DecisionTreeNode(label=labels[0])

    if not data:
        return None

    best_attribute, best_score, best_threshold = find_best_split(
        data, criterion)

    if best_score == 0:
        label_count = {}
        for label in labels:
            if label not in label_count:
                label_count[label] = 1
            else:
                label_count[label] += 1

        most_common_label = None
        most_common_count = -1
        for label, count in label_count.items():
            if count > most_common_count:
                most_common_count = count
                most_common_label = label
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


def evaluate_model(tree, test_data):
    true_labels = [label for _, label in test_data]
    predicted_labels = [predict(tree, sample) for sample, _ in test_data]

    precision = precision_score(true_labels, predicted_labels, average="macro")
    recall = recall_score(true_labels, predicted_labels, average="macro")
    f1 = f1_score(true_labels, predicted_labels, average="macro")

    return precision, recall, f1


def cross_validate(data, k, criterion="entropy"):
    kf = KFold(n_splits=k, shuffle=True, random_state=42)
    f1_scores = []

    for train_index, test_index in kf.split(data):
        train_data = [data[i] for i in train_index]
        test_data = [data[i] for i in test_index]

        decision_tree = build_decision_tree(train_data, criterion)

        precision, recall, f1 = evaluate_model(decision_tree, test_data)
        print(
            f"precision: {precision*100:.2f}%, recall: {recall*100:.2f}%, f1: {f1*100:.2f}%")
        f1_scores.append(f1)

    avg_f1 = sum(f1_scores) / len(f1_scores)
    print(f"Average F1 score: {avg_f1*100:.2f}%")


def print_tree(node, depth=0):
    if node is None:
        return

    if node.label is not None:
        print(" "*depth+"Leaf: " + str(label_map_index[node.label]))
    else:
        print(" "*depth+str(attribute_map[node.attribute_index]) +
              " <= "+str(node.threshold)+"?")

        print(" "*depth+"Left:")
        print_tree(node.left, depth+1)
        print(" "*depth+"Right:")
        print_tree(node.right, depth+1)


def main():
    dataset = load_data()
    # Choose "gini" or "entropy"
    cross_validate(dataset, 5, criterion="entropy")


if __name__ == "__main__":
    main()
