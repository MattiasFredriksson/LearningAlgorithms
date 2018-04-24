
import numpy as np
from math import *

#Read first column as names and n numbered columns
n=10 #N number colums
classifiers = np.genfromtxt("result.txt", usecols=0, dtype=None)
arr = np.loadtxt("result.txt", usecols=range(1,n+1), delimiter=",", dtype=None)

# Num. classifiers
k = len(classifiers)

# Evaluates the rank of a single value
def evalRank(col, ind):
    # Values less then the value increments it's rank.
    # Ties are evaluated as the average of the sum of the tied ranks
    #   This is a sum 1+2+3...n divided by the n ties
    #   Since the value is tied with itself there is a basecase 1/1=1 !
    rank = 0
    num_ties = 0
    for val in col:
        if val > col[ind]: rank += 1
        #Tied with itself
        elif val == col[ind]: num_ties += 1
    #end loop
    # Evaluate ties (Arithmetic series)
    seq_sum = 1/2.0*num_ties*(num_ties+1)
    rank += seq_sum / num_ties
    return rank;
#end evalRank()

# Evaluation
ranks = []
avg_ranks = []
average = []
std_dev = []

# Evaluate data
for i in range(k):
    avg = 0; rank = [0]*n; avg_rank = 0; variance = 0
    # Evaluate average
    for ii in range(n):
        avg += arr[i][ii]
        rank[ii] = evalRank(arr[:, ii], i)
        avg_rank += rank[ii]
    #end average iteration

    # Calc. & store
    avg = avg / n
    avg_rank = avg_rank / n
    average.append(avg)
    avg_ranks.append(avg_rank)
    ranks.append(rank)

    #Evaluate variance and standard deviation
    for val in arr[i]:
        variance += pow(val - avg, 2)
    #end variance loop
    variance /= n
    std_dev.append( sqrt(variance))
#end loop

#Freidman
a = 0.05
avg_rank = (k + 1) / 2.0
SS_t = 0
SS_e = 0
#Calc. avgs
for i in range(k):
    SS_t += pow(avg_ranks[i] - avg_rank, 2)
    for r in ranks[i]:
        SS_e += pow(r - avg_rank, 2)
    #end
#end

SS_t *= n
SS_e /= n*(k-1)
Q = SS_t / SS_e #Friedman statistic
CD = 2.343*sqrt(k*(k+1)/(6*n))

#Evaluate rank diff - CD : our 'significance matrix'
#(shortened with sign since the sign determines if significant or not ;)
sign_matrix = [[0]*k for i in range(k)]
for i in range(k):
    for j in range(k):
        if i == j: sign_matrix[i][j] = 0 #We dont compare with itself
        else: sign_matrix[i][j] = avg_ranks[j] - avg_ranks[i] - CD
#end

#Print result

#Print data table
line = ""
for i in range(k):
    line += classifiers[i].decode('UTF-8') + " \t"
print(line)

for ii in range(n):
    line = ""
    for i in range(k):
        line += str(round(arr[i][ii],4)) + " (" + str(ranks[i][ii]) + ")\t"
    print(line)
    #end for
#end for

# Print averages
def composeArr(arr, precision=12):
    line = ""
    for i in range(len(arr)-1):
        line += str(round(arr[i],precision)) + "\t"
        #line +=  "%.10f" % arr[i] + "\t"
    else:
        line += str(round(arr[len(arr)-1],precision)) + "\t"
    return line;
#end composeArr()

print("Avg. " + composeArr(average))
print("Std dev. " + composeArr(std_dev))
print("Avg. rank " + composeArr(avg_ranks))

print("Friedman statistic: ", Q)
print("Critical difference: ", CD, "\n")

#Print critical differences
print("Significance matrix:")
for i in range(k):
    line = ""
    for j in range(k):
        line += "%.4f" % sign_matrix[i][j] + ", "
    print(line)
    #end
#end

#done
