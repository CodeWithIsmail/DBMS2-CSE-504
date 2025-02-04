import sys
import math
import pandas as pd
import numpy as np


def getKFoldData(data, kfold):
    train_arr = list()
    test_arr = list()
    # data = pd.read_csv(path)
    dataLen = len(data)
    segmentSize = int(dataLen/kfold)
#     np.random.seed(kfold)
#     seedList = np.random.randint(low=0, high=200, size=kfold)

#     for seed in seedList:
#         tempDf = data.copy()
#         np.random.seed(seed)
#         tempDf.apply(np.random.shuffle, axis=0)
#         #print(tempDf.iloc[0:int(dataLen/10)])
#         test_arr.append(tempDf.iloc[0:segmentSize])
#         train_arr.append(tempDf.iloc[segmentSize: ].reset_index())
    tempDf = data.copy()
    np.random.seed(kfold)
    tempDf.apply(np.random.shuffle, axis=0)

    for i in range(0, kfold):

        # print(tempDf.iloc[0:int(dataLen/10)])
        testSegment = tempDf.iloc[(i*segmentSize): (i*segmentSize + segmentSize)]
        test_arr.append(testSegment)
        train_arr.append(tempDf.drop(
            tempDf.index[(i*segmentSize): (i*segmentSize + segmentSize)]))
        # train_arr.append(tempDf.iloc[int(dataLen/10): ].reset_index())

    return train_arr, test_arr


def remainder(df, df_subsets, predict_attr, classifiers):
    num_data = df.shape[0]
    remainder = float(0)
    for df_sub in df_subsets:
        if df_sub.shape[0] > 1:
            remainder += float(df_sub.shape[0]/num_data) * \
                info_entropy(df_sub, predict_attr, classifiers)
    return remainder


class Node(object):
    def __init__(self, attribute, threshold):
        self.attr = attribute
        self.thres = threshold
        self.left = None
        self.right = None
        self.leaf = False
        self.predict = None
        self.entropy = None


def info_entropy(df, predict_attr, classifiers):
    countList = num_class(df, predict_attr, classifiers)
    count = 0
    summation = 0
    for i in range(0, len(countList)):
        summation += countList[i]
        if countList[i] != 0:
            count += 1

    I = 0
    if count == 1:
        I = 0
    else:
        for co in countList:

            I += ((-1*co)/(summation))*math.log(co/(summation), 2)
    return I


def info_gain(df, attribute, predict_attr, threshold, classifiers):
    sub_1 = df[df.loc[:, attribute] < threshold]
    sub_2 = df[df.loc[:, attribute] > threshold]
    ig = info_entropy(df, predict_attr, classifiers) - \
        remainder(df, [sub_1, sub_2], predict_attr, classifiers)
    return ig


def select_threshold(df, attribute, predict_attr, classifiers):
    values = df.loc[:, attribute].tolist()
    values = [float(x) for x in values]
    values = set(values)
    values = list(values)
    values.sort()
    max_ig = float("-inf")
    thres_val = 0
    for i in range(0, len(values) - 1):
        thres = (values[i] + values[i+1])/2
        ig = info_gain(df, attribute, predict_attr, thres, classifiers)
        if ig > max_ig:
            max_ig = ig
            thres_val = thres
    return thres_val


def choose_attr(df, attributes, predict_attr, classifiers):
    max_info_gain = float("-inf")
    best_attr = None
    threshold = 0
    for attr in attributes:
        thres = select_threshold(df, attr, predict_attr, classifiers)
        ig = info_gain(df, attr, predict_attr, thres, classifiers)
        if ig > max_info_gain:
            max_info_gain = ig
            best_attr = attr
            threshold = thres
    return best_attr, threshold


def num_class(df, predict_attr, classifiers):
    countList = list()
    for i in classifiers:
        countList.append(df[df.loc[:, predict_attr] == i].shape[0])

    return countList


def build_tree(df, cols, predict_attr, classifiers):
    countList = num_class(df, predict_attr, classifiers)
    count = 0
    setClass = None
    for i in range(0, len(countList)):
        if countList[i] != 0:
            count += 1
            setClass = i

    if count == 1:
        leaf = Node(None, None)
        leaf.leaf = True
        leaf.predict = setClass
        return leaf
    else:
        best_attr, threshold = choose_attr(df, cols, predict_attr, classifiers)
        tree = Node(best_attr, threshold)
        tree.entropy = info_entropy(df, predict_attr, classifiers)
        sub_1 = df[df.loc[:, best_attr] < threshold]
        sub_2 = df[df.loc[:, best_attr] > threshold]
        tree.left = build_tree(sub_1, cols, predict_attr, classifiers)
        tree.right = build_tree(sub_2, cols, predict_attr, classifiers)
        return tree


def predict(node, row_df):
    if node.leaf:
        return node.predict
    if row_df.loc[node.attr] <= node.thres:
        return predict(node.left, row_df)
    elif row_df[node.attr] > node.thres:
        return predict(node.right, row_df)


def test_predictions(root, df, classifierName):
    num_data = df.shape[0]
    num_correct = 0
    for index, row in df.iterrows():
        prediction = predict(root, row)
        if prediction == row.loc[classifierName]:
            num_correct += 1
    return round(num_correct/num_data, 2)


def print_tree(root, level):
    # print(counter*" ", end="")
    if root.leaf:
        print("leaf predict: "+str(root.predict))
    else:
        print("node condition: "+root.attr+">" +
              str(root.thres) + "  entropy:"+str(root.entropy))
    if root.left:
        print_tree(root.left, level + 1)
    if root.right:
        print_tree(root.right, level + 1)


def clean(csv_file_name):
    df = pd.read_csv(csv_file_name, header=None)
    # df.columns = ['K', 'Na', 'CL', 'HCO', 'Endotoxin', 'Anioingap', 'PLA2', 'SDH', 'GLDH', 'TPP', 'Breath rate', 'PCV', 'Pulse rate', 'Fibrinogen', 'Dimer', 'FibPerDim', 'Diagnosis']
    cols = df.columns
    df[cols] = df[cols].apply(pd.to_numeric, errors='coerce')
    return df


df_train = clean('wine.data')
dataLen = df_train.shape[0]
train_arr = list()
test_arr = list()
kFold = None
if dataLen < 200:
    kFold = dataLen

else:
    kFold = 10
# print(df_train)
train_arr, test_arr = getKFoldData(df_train, kFold)
classifierName = 0
uniqueClassifier = list(range(0, df_train.loc[:, classifierName].nunique()))
# attributes =  ['K', 'Na', 'CL', 'HCO', 'Endotoxin', 'Anioingap', 'PLA2', 'SDH', 'GLDH', 'TPP', 'Breath rate', 'PCV', 'Pulse rate', 'Fibrinogen', 'Dimer', 'FibPerDim']
attributes = list(range(0, df_train.shape[1]))


attributes.remove(classifierName)
# print(attributes)
# for i in range(0, fr_train.shape[1]):
#     if i != int(classifierName):
#         attributes.append(str(i))


accuracy = 0

for i in range(0, kFold):
    root = build_tree(train_arr[i], attributes,
                      classifierName, uniqueClassifier)
# df_test = clean('horseTest.txt')
    accuracy += test_predictions(root, test_arr[i], classifierName)*100.0


print("accuracy: "+str(accuracy/kFold))
# print(str(test_predictions(root, df_test, classifierName)*100.0) + '%')
