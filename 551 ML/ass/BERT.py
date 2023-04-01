import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import nltk
from nltk.tokenize import word_tokenize
from transformers import BertTokenizer, BertForSequenceClassification, AdamW, get_linear_schedule_with_warmup
import torch
from torch.utils.data import TensorDataset, DataLoader, RandomSampler, SequentialSampler
import string
import re

train_df = pd.read_csv('train_data.csv', encoding='utf-8').iloc[0:1000]
test_df = pd.read_csv('test_data.csv', encoding='utf-8').iloc[0:1000]

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
Tokenization: Split the text into individual words (tokens). This step is essential 
for various text processing tasks, such as creating a bag-of-words representation or 
input for word embeddings.
"""
nltk.download('punkt') # Download the punkt tokenizer
train_df['review'] = train_df['review'].apply(word_tokenize)
test_df['review'] = test_df['review'].apply(word_tokenize)

print(train_df.describe())

train_texts, val_texts, train_labels, val_labels = train_test_split(train_df['review'], train_df['sentiment'], test_size=0.1, random_state=42)

tokenizer = BertTokenizer.from_pretrained('bert-base-uncased') # Load the BERT tokenizer. 

def encode_texts(texts, max_len=256):
    """
    Tokenize the texts and map the tokens to their word IDs.
    attention_masks indicate to the model which tokens should be attended to, 
    which is just the tokens with real content and not the padding tokens.
    """
    input_ids = []
    attention_masks = []

    for text in texts:
        tokens = tokenizer.encode_plus(
            text,
            max_length=max_len,
            pad_to_max_length=True,
            truncation=True,
            return_attention_mask=True,
            return_tensors='pt'
        )
        input_ids.append(tokens['input_ids'])
        attention_masks.append(tokens['attention_mask'])

    input_ids = torch.cat(input_ids, dim=0)
    attention_masks = torch.cat(attention_masks, dim=0)

    return input_ids, attention_masks

train_input_ids, train_attention_masks = encode_texts(train_texts) 
val_input_ids, val_attention_masks = encode_texts(val_texts)

# Convert the labels to pytorch tensors
train_dataset = TensorDataset(train_input_ids, train_attention_masks, torch.tensor(train_labels.values))
val_dataset = TensorDataset(val_input_ids, val_attention_masks, torch.tensor(val_labels.values))
print("Training dataset size: ", len(train_dataset))
print("Validation dataset size: ", len(val_dataset))
batch_size = 16

# Create the DataLoaders for our training and validation sets.
train_dataloader = DataLoader(train_dataset, sampler=RandomSampler(train_dataset), batch_size=batch_size)
val_dataloader = DataLoader(val_dataset, sampler=SequentialSampler(val_dataset), batch_size=batch_size)
print("Training dataloader size: ", len(train_dataloader))
print("Validation dataloader size: ", len(val_dataloader))

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"device is {device}!!!")
# Load BertForSequenceClassification, the pretrained BERT model with a single linear classification layer on top.
model = BertForSequenceClassification.from_pretrained("bert-base-uncased", num_labels=2, output_attentions=True)
model.to(device)

# AdamW is a class from the huggingface library (as opposed to pytorch). It uses weight decay instead of L2 regularization.
optimizer = AdamW(model.parameters(), lr=2e-5, eps=1e-8) # Default parameters and optimization algorithm
num_epochs = 1
total_steps = len(train_dataloader) * num_epochs
# Create the learning rate scheduler. It is used to decrease the learning rate as the training progresses.
scheduler = get_linear_schedule_with_warmup(optimizer, num_warmup_steps=0, num_training_steps=total_steps)

def train_epoch(model, dataloader, optimizer, scheduler, device):
    model.train() # model is the BertForSequenceClassification model
    for step, batch in enumerate(dataloader):
        input_ids, attention_masks, labels = batch
        input_ids = input_ids.to(device)
        attention_masks = attention_masks.to(device)
        labels = labels.to(device)

        model.zero_grad() # Clear any previously calculated gradients before performing a backward pass.
        outputs = model(input_ids, attention_mask=attention_masks, labels=labels)
        loss = outputs.loss
        loss.backward() # update gradients
        print(f"Batch {step} of {len(dataloader)} loss: {loss.item()}")
        
        # Clip norm of gradients to 1.0. helps prevent the exploding gradients
        torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0) 

        optimizer.step() # Update parameters and take a step using the computed gradient.
        scheduler.step() # Update the learning rate.

def evaluate(model, dataloader, device):
    model.eval()
    total_eval_accuracy = 0

    for batch in dataloader:
        input_ids, attention_masks, labels = batch
        input_ids = input_ids.to(device)
        attention_masks = attention_masks.to(device)
        labels = labels.to(device)

        with torch.no_grad():
            outputs = model(input_ids, attention_mask=attention_masks, labels=labels)

        logits = outputs.logits
        preds = torch.argmax(logits, dim=1).cpu().numpy()
        total_eval_accuracy += accuracy_score(labels.cpu().numpy(), preds)
    
    correct_indices = []
    incorrect_indices = []

    for i, (labels_np, preds) in enumerate(zip(labels.cpu().numpy(), preds)):
        if labels_np == preds:
            correct_indices.append(i)
        else:
            incorrect_indices.append(i)

    avg_val_accuracy = total_eval_accuracy / len(dataloader)
    return avg_val_accuracy, correct_indices, incorrect_indices

# Train the model
for epoch in range(num_epochs):
    print(f"Epoch {epoch+1}/{num_epochs}")
    print("-" * 10)

    train_epoch(model, train_dataloader, optimizer, scheduler, device)
    val_accuracy, val_correct_indices, val_incorrect_indices = evaluate(model, val_dataloader, device)

    print(f"Validation accuracy: {val_accuracy:.4f}")

test_texts = test_df['review']
test_labels = test_df['sentiment']

test_input_ids, test_attention_masks = encode_texts(test_texts)
test_dataset = TensorDataset(test_input_ids, test_attention_masks, torch.tensor(test_labels.values))
test_dataloader = DataLoader(test_dataset, sampler=SequentialSampler(test_dataset), batch_size=batch_size)

test_accuracy, test_correct_indices, test_incorrect_indices = evaluate(model, test_dataloader, device)
print(f"Test accuracy: {test_accuracy:.4f}")
    
import matplotlib.pyplot as plt
def get_attention_matrix(model, input_ids, attention_masks, block_idx, head_idx, device):
    input_ids = input_ids.to(device)
    attention_masks = attention_masks.to(device)
    model.eval()
    with torch.no_grad():
        outputs = model(input_ids, attention_mask=attention_masks)
        attentions = outputs.attentions
    attention_matrix = attentions[block_idx][0, head_idx].detach().cpu().numpy()
    return attention_matrix
def visualize_attention(input_id, attention_matrix):
    tokens = tokenizer.convert_ids_to_tokens(input_id)
    plt.imshow(attention_matrix, cmap="viridis")
    plt.xticks(range(len(tokens)), tokens, rotation=90)
    plt.yticks(range(len(tokens)), tokens)
    plt.show()
    
block_idx = 0
head_idx = 0
num_examples = 5
correct_examples = [val_input_ids[i] for i in val_correct_indices[:num_examples]]
correct_attention_masks = [val_attention_masks[i] for i in val_correct_indices[:num_examples]]
incorrect_examples = [val_input_ids[i] for i in val_incorrect_indices[:num_examples]]
incorrect_attention_masks = [val_attention_masks[i] for i in val_incorrect_indices[:num_examples]]

correct_attention_matrices = [
    get_attention_matrix(model, input_id.unsqueeze(0), attention_mask.unsqueeze(0), block_idx, head_idx, device)
    for input_id, attention_mask in zip(correct_examples, correct_attention_masks)
]

incorrect_attention_matrices = [
    get_attention_matrix(model, input_id.unsqueeze(0), attention_mask.unsqueeze(0), block_idx, head_idx, device)
    for input_id, attention_mask in zip(incorrect_examples, incorrect_attention_masks)
]

for input_id, attention_matrix in zip(correct_examples, correct_attention_matrices):
    visualize_attention(input_id, attention_matrix)

for input_id, attention_matrix in zip(incorrect_examples, incorrect_attention_matrices):
    visualize_attention(input_id, attention_matrix)