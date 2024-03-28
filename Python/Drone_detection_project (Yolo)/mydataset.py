import torch
import torch.nn as nn
import torch.nn.functional as F
from math import sqrt
from torch.utils.data import Dataset, Subset, DataLoader, random_split
from torchvision import datasets, models, transforms

class MyDataset(Dataset):
    def __init__(self, imgs, labels):
        self.imgs = imgs
        self.labels = labels
        self.transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Resize((1080, 1920)),
            transforms.Normalize([0.2859], [0.3530])
        ])

    def __len__(self):
        return len(self.imgs)
    
    def __getitem__(self, idx):
        img = self.imgs[idx]
        label = self.labels[idx]
        img = self.transform(img)
        sample = {'img': img, 'label': label}
        return sample