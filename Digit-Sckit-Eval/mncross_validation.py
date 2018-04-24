import numpy as np
import functools
import sys
from sklearn import datasets, metrics, tree, neighbors, svm, naive_bayes, cluster
from sklearn.model_selection import StratifiedKFold
from sklearn.preprocessing import StandardScaler
from timeit import default_timer as timer

# n test runs for every specified classifier
n = 1
folds = 3;

 #0: Time, 1: Accurracy, 2: F-measure
measure_var = 1
try:
    measure_var = int(sys.argv[1])
except:
    measure_var = 0
metric_used = [1,2] #Time is always used
metric_names = ["Time", "Accuracy", "F-measure"]

X = np.load("features_train.npy")
y = np.load("label_train.npy")

#Normalization scaler
scaler = StandardScaler()

#Model selector
skf = StratifiedKFold(n_splits=folds, shuffle=True)
#Evaluation method
metric_list = [
 functools.partial(metrics.accuracy_score), #Accuracy
 functools.partial(metrics.f1_score, average='micro', pos_label=1) #F-score
]
metric = [] # List metrics used
for i in metric_used:
    metric.append(metric_list[i-1])
#end

classifiers = [
neighbors.KNeighborsClassifier(n_neighbors=5, weights='distance'),
#cluster.KMeans(n_clusters=5, random_state=0),
#tree.DecisionTreeClassifier(),
#svm.SVC(max_iter=-1, kernel='linear', class_weight=None),
#svm.SVC(max_iter=-1, kernel='rbf', class_weight=None),
#naive_bayes.GaussianNB(priors=None)
]

# Result lists
# List containing a 2d array of fold x classifier dimension, per metric
# (including time)
result = []
for i in range(0, len(metric_used)+1):
    result.append([[0] * folds for foo in range(0, len(classifiers))])
#endfor

print("\nRunning ", n, " tests...\n")
run_start = timer()
#Perform n tests
for i in range(0,n):
    # Generate folds:
    split = skf.split(X, y);
    fold = 0
    print("\Test ", i, " ...\n")
    fold_timer = timer()
    # Run tests for every classifier on every fold:
    for train_ind, val_ind in split:
        #Preprocess fold
        process_timer = timer()
        X_train = X[train_ind]; y_train = y[train_ind]
        X_val = X[val_ind]; y_val = y[val_ind]
        scaler.fit(X_train);
        X_train = scaler.transform(X_train)
        X_val = scaler.transform(X_val)
        print("Process time: ", process_timer)
        for clf_i in range(0,len(classifiers)):
            print("1 : ",timer())
            # Run test:
            clf = classifiers[clf_i]
            print("2 : ",timer())
            start = timer()
            clf.fit(X_train, y_train)
            #Evaluate time:
            result[0][clf_i][fold] += (timer()-start) / n
            # Evaluate metrics:
            print("3 : ",timer())
            print("Len Train : ",size(X_train,0))
            print("Len Val : ",size(X_val,0))
            y_res = clf.predict(X_val)
            print("4 : ",timer())
            for ii in range(0, len(metric)):
                res = metric[ii](y_val, y_res)      #Calc. metric
                result[ii+1][clf_i][fold] += res / n; #Average and store
            #end metric iteration
            print("5 : ",timer())
        #end classifier iteration
        fold += 1
        print("Fold ", fold , " complete, time: ", (timer()-fold_timer))
    #end fold iterations
    print("Test ", i+1 , " complete...")
#end dataset iteration

#If we measured time!
if measure_var == 2: result = timed

metric_used.insert(0,0) #Always use time metric
for m in metric_used:
    print("\n")
    l_res = result[m]
    for i in range(0,len(classifiers)):
        print(classifiers[i].__class__.__name__)
        print("Result: ", metric_names[m])
        print(l_res[i], "\n")

    #Store result
    file = open("res_" + metric_names[m] + ".txt","w")

    for i in range(0,len(classifiers)):
        line = classifiers[i].__class__.__name__ + ", "
        for ii in range(len(l_res[i])-1):
            line += str(l_res[i][ii]) + ", "
        line += str(l_res[i][len(l_res[i])-1]) + "\n"
        file.write(line)

    file.close()
#end metric print

print("Complete! Runtime: ", timer()-run_start)
