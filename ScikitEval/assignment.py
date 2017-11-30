import numpy as np
import functools
from sklearn import datasets, metrics, tree, neighbors, svm, naive_bayes
from sklearn.model_selection import StratifiedKFold
from sklearn.preprocessing import StandardScaler
from timeit import default_timer as timer

# n test runs for every specified classifier
n = 10
folds = 10;
measure_var = 0 #0: F-measure, 1: Accurracy, 2: Time
m_names = ["F-measure", "Accurracy", "Time"]

X = np.load("spambase_data.npy")
y = np.load("spambase_target.npy")

#Normalize
scaler = StandardScaler()
#scaler.fit_transform(X)

#Model selector
skf = StratifiedKFold(n_splits=folds, shuffle=True)
#Evaluation method
metric = functools.partial(metrics.f1_score, average='binary', pos_label=1)
if measure_var == 1: metric = functools.partial(metrics.accuracy_score)


classifiers = [
#neighbors.KNeighborsClassifier(n_neighbors=5, weights='distance'),
tree.DecisionTreeClassifier(),
svm.SVC(max_iter=-1, kernel='linear', class_weight=None),
naive_bayes.GaussianNB(priors=None)
]

# Result lists (arr of 1 value for each fold per classifier)
result = [[0] * folds for foo in range(0, len(classifiers))]
timed = [[0] * folds for foo in range(0, len(classifiers))]

print("\nRunning ", n, " tests...\n")
run_start = timer()
#Perform n tests
for i in range(0,n):
    # Generate folds:
    split = skf.split(X, y);
    fold = 0
    # Run tests for every classifier on every fold:
    for train_ind, val_ind in split:
        #Preprocess fold
        X_train = X[train_ind]; y_train = y[train_ind]
        X_val = X[val_ind]; y_val = y[val_ind]
        scaler.fit(X_train);
        X_train = scaler.transform(X_train)
        X_val = scaler.transform(X_val)
        for clf_i in range(0,len(classifiers)):
            # Run test:
            start = timer()
            clf = classifiers[clf_i]
            clf.fit(X_train, y_train)
            # Evaluate:
            y_res = clf.predict(X_val)
            res = metric(y_val, y_res)
            result[clf_i][fold] += res / n; #Average and store
            timed[clf_i][fold] += (timer()-start) / n #Time
        #end classifier iteration
        fold += 1
    #end fold iterations
    print("Test ", i+1 , " complete...")
#end dataset iteration

#If we measured time!
if measure_var == 2: result = timed

print("\n")
for i in range(0,len(classifiers)):
    print(classifiers[i].__class__.__name__)
    print("Result: ", m_names[measure_var])
    print(result[i], "\n")
    #print("Time:")
    #print(timed[i], "\n")
#Store result
file = open("result.txt","w")

for i in range(0,len(classifiers)):
    line = classifiers[i].__class__.__name__ + ", "
    for ii in range(len(result[i])-1):
        line += str(result[i][ii]) + ", "
    line += str(result[i][len(result[i])-1]) + "\n"
    file.write(line)

file.close()

print("Complete! Runtime: ", timer()-run_start)
