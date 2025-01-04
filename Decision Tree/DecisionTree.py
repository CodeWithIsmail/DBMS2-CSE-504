import math
from sklearn.model_selection import KFold

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


def split_data(data, attribute_index, threashold):
    left = []
    right = []
    for row in data:
        value = row[0][attribute_index]
        if value <= threashold:
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


def calculate_information_gain(data, attribute_index, threashold):
    left, right = split_data(data, attribute_index, threashold)
    left_entropy = calculate_entropy(left)
    right_entropy = calculate_entropy(right)
    parent_entropy = calculate_entropy(data)
    left_w = len(left)/len(data)
    right_w = len(right)/len(data)
    weighted_entropy = left_w*left_entropy+right_w*right_entropy
    information_gain = parent_entropy-weighted_entropy
    return information_gain


def find_best_split(data):
    best_gain = 0
    best_attribute = None
    best_threshold = None
    n_features = len(data[0][0])

    for attribute_index in range(n_features):
        values = set([row[0][attribute_index] for row in data])
        for threshold in values:
            gain = calculate_information_gain(
                data, attribute_index, threshold)
            if gain > best_gain:
                best_gain = gain
                best_attribute = attribute_index
                best_threshold = threshold
    return best_attribute, best_gain, best_threshold


def build_decision_tree(data, depth=0):
    labels = [row[1] for row in data]

    if len(set(labels)) == 1:
        return DecisionTreeNode(label=labels[0])

    if not data:
        return None

    best_attribute, best_gain, best_threshold = find_best_split(data)

    if best_gain == 0:
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
    left_subtree = build_decision_tree(left_data, depth+1)
    right_subtree = build_decision_tree(right_data, depth+1)

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
    correct = 0
    for sample, label in test_data:
        predict_label = predict(tree, sample)
        if predict_label == label:
            correct += 1
    accuracy = correct/len(test_data)
    return accuracy


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


def cross_validate(data, k):
    kf = KFold(n_splits=k, shuffle=True, random_state=42)
    accuracies = []

    for train_index, test_index in kf.split(data):
        train_data = [data[i] for i in train_index]
        test_data = [data[i] for i in test_index]

        decision_tree = build_decision_tree(train_data)
        # print_tree(decision_tree)
        accuracy = evaluate_model(decision_tree, test_data)
        print(f"accuracy: {accuracy*100:.2f}%")
        accuracies.append(accuracy)

    avg_accuracy = sum(accuracies)/len(accuracies)
    print(f"average accuracy: {avg_accuracy*100:.2f}%")


def main():
    dataset = load_data()
    cross_validate(dataset, 5)


if __name__ == "__main__":
    main()
