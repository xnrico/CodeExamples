import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader, random_split
from torchvision import transforms

# Define the neural network architecture
class Net(nn.Module):

    def __init__(self):
        super(Net, self).__init__()

        # Define layers and operations
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool2d(2, stride=2)
        self.conv1 = nn.Conv2d(3, 64, 5, padding=2, stride=2)
        self.conv2 = nn.Conv2d(64, 16, 5, padding=2, stride=2)
        self.conv3 = nn.Conv2d(16, 1, 5, padding=2, stride=2)
        self.fc1 = nn.Linear(in_features=510, out_features=2)

    def forward(self, x):
        # Forward pass through the network
        x = self.conv1(x)
        x = self.relu(x)
        x = self.pool(x)
        x = self.conv2(x)
        x = self.relu(x)
        x = self.pool(x)
        x = self.conv3(x)
        x = self.relu(x)
        x = torch.flatten(x, 1, -1)
        x = self.fc1(x)

        return x