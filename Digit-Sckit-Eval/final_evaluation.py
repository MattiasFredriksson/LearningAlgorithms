import numpy as np
import functools
import sys
from sklearn import datasets, metrics, tree, neighbors, svm, naive_bayes
from sklearn.model_selection import StratifiedKFold
from sklearn.preprocessing import StandardScaler
from timeit import default_timer as timer

# n test runs for every specified classifier
n = 1

#Metrics
#0: Time, 1: Accurracy, 2: F-measure
metric_used = [1,2] #Time is always used
metric_names = ["Time", "Acc", "F-meas"]

direc = "feature/"
X_train = np.load(direc + "features_train.npy")
y_train = np.load(direc + "label_train.npy")
X_test = np.load(direc + "features_test.npy")
y_test = np.load(direc + "label_test.npy")

print("Training dim.: ", X_train.shape[0])
print("Test dim.: ", X_test.shape[0])
print("Features: ", X_train.shape[1])

#Normalization scaler
scaler = StandardScaler()
scaler.fit(X_train);
X_train = scaler.transform(X_train)
X_test = scaler.transform(X_test)

#Evaluation method
metric_list = [
 functools.partial(metrics.accuracy_score), #Accuracy
 functools.partial(metrics.f1_score, average='micro', pos_label=1) #F-score
]
metric = [] # List metrics used
for i in metric_used:
    if i > 0:
        metric.append(metric_list[i-1])
    #endif
#end

classifiers = [
neighbors.KNeighborsClassifier(n_neighbors=5, weights='distance'),
#tree.DecisionTreeClassifier(),
#svm.SVC(max_iter=-1, kernel='rbf', class_weight=None)
]

# Result lists
result = []
for i in range(0, len(classifiers)):
    result.append([0] * (len(metric_used)+1))
#endfor

#Print some test setup info:
print("\nRunning ", n, " tests on:")
for i in range(len(classifiers)):
    print(classifiers[i].__class__.__name__)
    #end
print("") #endl

run_start = timer()
#Perform n tests
for i in range(0,n):
    # Run test
    for clf_i in range(0,len(classifiers)):
        print("Classifier: ", classifiers[clf_i].__class__.__name__)
        # Run test:
        clf = classifiers[clf_i]
        start = timer()
        clf.fit(X_train, y_train)
        #Evaluate time:
        result[clf_i][0] += (timer()-start) / n
        # Evaluate metrics:
        y_res = clf.predict(X_test)
        for ii in range(0, len(metric)):
            res = metric[ii](y_test, y_res)      #Calc. metric
            result[clf_i][ii+1] += res / n; #Average and store
        #end metric iteration
    #end classifier iteration
    print("Run ", i+1 , " complete...")
#end dataset iteration

metric_used.insert(0,0) #Always use time metric
print("\n")
metric_list = metric_names[0] #[classifiers[0].__class__.__name__]
for m in metric_used:
    metric_list += " \t" + metric_names[m]
#end

for i in range(len(classifiers)):
    print(classifiers[i].__class__.__name__, " \t" , result[i])
#end

#Store result
file = open("result.txt","w")

file.write(metric_list + "\n")
for i in range(len(classifiers)):
    line = classifiers[i].__class__.__name__ + "\t "
    line += str(result[i][0])
    for ii in range(1,len(result[i])):
        line += " \t" + str(result[i][ii])
    file.write(line + "\n")
#end
file.close()
#end metric print

print("Complete! Runtime: ", timer()-run_start)
