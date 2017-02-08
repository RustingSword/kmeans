#!/usr/bin/env python

import sys
from matplotlib import pyplot as plt
from matplotlib import cm
import numpy as np

plt.style.use('ggplot')

def pca(X, no_dims=2):
    if X.shape[1] <= no_dims:
        return X

    print "Preprocessing the data using PCA..."
    (n, d) = X.shape
    X = X - np.tile(np.mean(X, 0), (n, 1))
    (l, M) = np.linalg.eig(np.dot(X.T, X))
    Y = np.dot(X, M[:, 0:no_dims])
    return Y

def plot():
    if len(sys.argv) != 4:
        print 'Usage:', sys.argv[0], '<data> <labels> <centers>'
        sys.exit(-1)
    data = pca(np.loadtxt(sys.argv[1]))
    labels = np.loadtxt(sys.argv[2])
    centers = pca(np.loadtxt(sys.argv[3]))
    assert len(data) == len(labels)

    # via http://stackoverflow.com/a/12236808
    colors = iter(cm.rainbow(np.linspace(0, 1, len(centers))))

    for cluster_id in range(len(centers)):
        samples = data[labels==cluster_id]
        color = next(colors)
        plt.scatter(samples[:,0], samples[:,1], color=color,
                label='cluster_' + str(cluster_id), alpha=0.5)
        plt.scatter(centers[cluster_id,0], centers[cluster_id,1], color='black',
                lw=2, marker='x', s=40)
    plt.title('Cluster result')
    plt.legend(loc='best')
    plt.show()

if __name__ == '__main__':
    plot()
