import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import train_test_split
import sklearn
import math
import torch
import cv2
import time
import os
import string
import re
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from sklearn.feature_extraction.text import CountVectorizer
from multiprocessing import Pool, cpu_count
from collections import defaultdict, Counter # For word counts
from concurrent.futures import ThreadPoolExecutor

# Define path to data
# data_dir = "C:/Users/SLsec/Downloads/aclImdb/"

# def load_imdb_dataset(imdb_folder, data_type):
#     data = {'review': [], 'sentiment': []}
#     for sentiment in ['pos', 'neg']:
#         sentiment_folder = os.path.join(imdb_folder, data_type, sentiment)
#         for review_file in os.listdir(sentiment_folder):
#             with open(os.path.join(sentiment_folder, review_file), 'r', encoding='utf-8') as f:
#                 data['review'].append(f.read())
#                 data['sentiment'].append(1 if sentiment == 'pos' else 0)
#     return pd.DataFrame(data)

# # Load the train and test datasets
# train_df = load_imdb_dataset('C:/Users/SLsec/Downloads/aclImdb', 'train')
# test_df = load_imdb_dataset('C:/Users/SLsec/Downloads/aclImdb', 'test')

# # Shuffle the datasets
# train_df = train_df.sample(frac=1, random_state=42).reset_index(drop=True)
# test_df = test_df.sample(frac=1, random_state=42).reset_index(drop=True)

# # Display the first few records of the train dataset
# print(train_df.head())

# # Save the train and test datasets as CSV files
# train_df.to_csv('train_data.csv', index=False, encoding='utf-8')
# test_df.to_csv('test_data.csv', index=False, encoding='utf-8')

# Load the train and test datasets from CSV files
train_df = pd.read_csv('train_data.csv', encoding='utf-8')
test_df = pd.read_csv('test_data.csv', encoding='utf-8')

# Display the first few records of the train dataset
print(train_df.head())

"""
Convert all text to lowercase to reduce the dimensionality 
of the data and treat words in different cases as the same token.
"""
train_df['review'] = train_df['review'].str.lower()
test_df['review'] = test_df['review'].str.lower()

"""
Removing HTML tags: The dataset may contain HTML tags that should be removed before analysis.
"""
def remove_html_tags(text):
    return re.sub('<[^>]*>', '', text)
train_df['review'] = train_df['review'].apply(remove_html_tags)
test_df['review'] = test_df['review'].apply(remove_html_tags)

"""
Removing punctuation and special characters: These characters may not be relevant 
for sentiment analysis and can be removed to simplify the text.
"""
def remove_punctuation(text):
    return text.translate(str.maketrans('', '', string.punctuation))
train_df['review'] = train_df['review'].apply(remove_punctuation)
test_df['review'] = test_df['review'].apply(remove_punctuation)

"""
Tokenization: Split the text into individual words (tokens). essential 
for various text processing tasks, such as creating a bag-of-words representation or 
input for word embeddings.
"""
nltk.download('punkt') # Download the punkt tokenizer 
train_df['review'] = train_df['review'].apply(word_tokenize)
test_df['review'] = test_df['review'].apply(word_tokenize)

class NaiveBayes:
    def __init__(self):
        self.classes = None # List of classes
        self.class_counts = None # Dictionary of class counts
        self.word_counts = None # Dictionary of word counts
        self.vocabulary = None # Set of unique words

    def fit(self, X, y, alpha_val):
        t2 = time.time()
        print(f"Training the model... X: {X.shape}, y: {y.shape}")
        self.classes, class_counts = np.unique(y, return_counts=True) # Get the list of classes and their counts
        self.class_counts = dict(zip(self.classes, class_counts)) # key: class, value: count of class
        # print(f"Classes: {self.classes}")
        self.vocabulary = set(word for review in X for word in review) # Get the set of unique words
        # print(f"Vocabulary size: {len(self.vocabulary)}")
        self.word_counts = {c: {word: 0 for word in self.vocabulary} for c in self.classes}
        # print(f"Word counts: {self.word_counts}")

        # Count the number of times each word appears in each class. Zip returns an iterator of tuples
        for x, label in zip(X, y): 
            for word in x: 
                self.word_counts[label][word] += 1
        
        # Precompute logarithms of prior probabilities and word probabilities 
        self.log_priors = {} # Dictionary of log priors
        self.log_word_probs = defaultdict(lambda: defaultdict(float)) # Dictionary of log word probabilities
        for c in self.classes:
            self.log_priors[c] = np.log(self.class_counts[c] / sum(self.class_counts.values()))
            total_words = sum(self.word_counts[c].values()) + len(self.vocabulary)
            for word in self.vocabulary:
                word_count = self.word_counts[c][word] + alpha_val  # Additive (Laplace) smoothing
                self.log_word_probs[c][word] = np.log(word_count / total_words)
        print("done training model!! :D")
        print("training time: ", time.time()-t2)
                
    def predict(self, X):
        t1 = time.time()
        print(f"Predicting... X: {X.shape}")
        predictions = []
        i = 0
        for x in X:
            probabilities = []
            x_unique = set(x)  # Only loop through unique words in the sample
            for c in self.classes:
                log_prob = self.log_priors[c] 
                for word in x_unique:
                    if word in self.vocabulary:
                        log_prob += x.count(word) * self.log_word_probs[c][word] # Additive (Laplace) smoothing
                probabilities.append(log_prob)
            predictions.append(self.classes[np.argmax(probabilities)])
            i+=1
            if i%10000 == 0:
                print(str(i)+") probabilities: ", probabilities, "time: ", time.time()-t1)
        return predictions
    
    def evaluate_acc(self, y_true, y_pred):
        # correct = sum(y_true == y_pred)
        # total = len(y_true)
        # return correct / total
        # if type(y_pred) != np.ndarray:
        if not isinstance(y_pred, np.ndarray):
          y_pred = np.array(y_pred)
        tp = sum((y_true == 1) & (y_pred == 1))
        fp = sum((y_true == 0) & (y_pred == 1))
        tn = sum((y_true == 0) & (y_pred == 0))
        fn = sum((y_true == 1) & (y_pred == 0))

        accuracy = (tp + tn)/(tp + tn + fp + fn)
        precision = tp / (tp + fp)
        recall = tp / (tp + fn)
        f1_score = 2 * (precision * recall) / (precision + recall)

        return accuracy, precision, recall, f1_score

def train_evaluate_alpha(alpha_value):
    X_train = train_df['review'].values
    y_train = train_df['sentiment'].values
    X_test = test_df['review'].values
    y_test = test_df['sentiment'].values

    nb_classifier = NaiveBayes()
    nb_classifier.fit(X_train, y_train, alpha_value)

    num = 25000
    y_pred = nb_classifier.predict(X_test[:num])

    accuracy, precision, recall, f1_score = nb_classifier.evaluate_acc(y_test[:num], y_pred)
    print("Alpha: %d, Accuracy: %.4f, Precision: %.4f, Recall: %.4f, F1_score: %.4f" % (alpha_value, accuracy, precision, recall, f1_score))
    return [accuracy, precision, recall, f1_score]
 
alpha_values = np.arange(20) * 10

with ThreadPoolExecutor() as executor:
    results = list(executor.map(train_evaluate_alpha, alpha_values))

# extract accuracy scores
accuracies = [result[0] for result in results]
Precisions = [result[1] for result in results]
Recalls = [result[2] for result in results]
F1_scores = [result[3] for result in results]
# plot accuracy scores
plt.plot(alpha_values, accuracies)
plt.xlabel('alpha values')
plt.ylabel('accuracy')
plt.title('Accuracy Scores vs. Alpha Values')
plt.show()

plt.plot(alpha_values,Precisions)
plt.xlabel('alpha values')
plt.ylabel('Precision')
plt.title('Precision Scores vs. Alpha Values')
plt.show()

plt.plot(alpha_values, Recalls)
plt.xlabel('alpha values')
plt.ylabel('Recall')
plt.title('Recall Scores vs. Alpha Values')
plt.show()

plt.plot(alpha_values, F1_scores)
plt.xlabel('alpha values')
plt.ylabel('accuracy')
plt.title('f1_score Scores vs. Alpha Values')
plt.show()