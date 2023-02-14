def unpickle(file):
    import pickle
    with open(file, 'rb') as fo:
        dict = pickle.load(fo, encoding='bytes')
    return dict

file = 'cifar/data_batch_2'
a = unpickle(file)
print(type(a))
print(a[0])
