import os
import time
import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from net_class import Net
from mydataset import MyDataset
from PIL import Image
from sklearn.metrics import average_precision_score as ap_score
from torch.utils.data import Dataset, Subset, DataLoader, random_split
from torchvision import datasets, models, transforms
from tqdm import tqdm  

class FastRCNN_ResNet(nn.Module):
    def __init__(self):
        super(FastRCNN_ResNet, self).__init__()
        self.resnet = models.resnet50(pretrained=True)
        self.pooling = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(2048, 2)

    def forward(self, x):
        features = self.resnet(x)
        pooled_features = self.pooling(features)
        pooled_features = pooled_features.view(pooled_features.size(0), -1)
        out = self.fc(pooled_features)
        return out


def train(model, dataloader, optimizer, criterion, num_epochs):
    for epoch in range(num_epochs):
        model.train()
        running_loss = 0.0
        for i, data in enumerate(dataloader):
            images, rois, labels = data

            optimizer.zero_grad()

            outputs = model(images, rois)
            loss = criterion(outputs, labels)

            loss.backward()
            optimizer.step()

            running_loss += loss.item()

            print('[%d, %5d] loss: %.3f' % (epoch + 1, i + 1, running_loss / 10))
            running_loss = 0.0


def get_iou(ground_truth, pred):
    # coordinates of the area of intersection.
    ix1 = np.maximum(ground_truth[0], pred[0])
    iy1 = np.maximum(ground_truth[1], pred[1])
    ix2 = np.minimum(ground_truth[2], pred[2])
    iy2 = np.minimum(ground_truth[3], pred[3])
     
    # Intersection height and width.
    i_height = np.maximum(iy2 - iy1 + 1, np.array(0.))
    i_width = np.maximum(ix2 - ix1 + 1, np.array(0.))
     
    area_of_intersection = i_height * i_width
     
    # Ground Truth dimensions.
    gt_height = ground_truth[3] - ground_truth[1] + 1
    gt_width = ground_truth[2] - ground_truth[0] + 1
     
    # Prediction dimensions.
    pd_height = pred[3] - pred[1] + 1
    pd_width = pred[2] - pred[0] + 1
     
    area_of_union = gt_height * gt_width + pd_height * pd_width - area_of_intersection
     
    iou = area_of_intersection / area_of_union
     
    return iou

def evaluate(model, data_loader):
    model.eval()
    predictions = []
    targets = []
    with torch.no_grad():
        for images, target in data_loader:
            outputs = model(images)
            for i, output in enumerate(outputs):
                boxes = output['boxes'].data.numpy()
                scores = output['scores'].data.numpy()
                pred = boxes[np.argmax(scores)]
                predictions.append(pred)
                target_box = target[i]['boxes'].data.numpy()[0]
                targets.append(target_box)

    predictions = np.array(predictions)
    targets = np.array(targets)

    mean_iou = get_iou(targets, predictions)

    print("Mean IOU: ", mean_iou)

    return mean_iou

def main():
    batch_size = 50

    cam0_path = "data/labels/cam0.txt"
    data = np.loadtxt(cam0_path)
    x,y = data[:,1],data[:,2]
    cam0_labels = np.array(list(zip(x, y)))
    cam0_labels = cam0_labels[:2000]
    image_dir = '/Users/kritimoogala/Desktop/eecs442/442finalproject/data/frames/cam0'
    count = sum(1 for f in os.listdir(image_dir) if os.path.isfile(os.path.join(image_dir, f)) and not f.startswith('.'))
    count = 1000
    cam0_images = np.zeros((count, 1080, 1920, 3))
    for i in range(count):  
        img = ('/frame%d.jpg' % i)
        cam0_images[i] = Image.open(image_dir + img).convert('RGB')
    print('loaded imgs and labels')

    data = MyDataset(cam0_images, cam0_labels)
    print('created dataset')

    #referenced from here: https://stackoverflow.com/questions/71576668/in-pytorch-how-can-i-shuffle-a-dataloader
    #length of the total dataset
    N = count
    
    indices = np.arange(N)
    indices = np.random.permutation(indices)

    #shuffling data before creating subsets
    #dividing into 70, 15, 15 for train, val, test
    train_indices = indices [:int(0.7*N)]
    val_indices = indices[int(0.7*N):int(0.85*N)]
    test_indices = indices[int(0.85*N):]

    train_vals = Subset(data, train_indices)
    valid_vals = Subset(data, val_indices)
    test_vals = Subset(data, test_indices)

    print('created vals')

    train_loader = DataLoader(train_vals, batch_size, shuffle=True)
    val_loader = DataLoader(valid_vals, batch_size, shuffle=True)
    test_loader = DataLoader(test_vals, batch_size, shuffle=True)

    print('created loaders')

    # Define the model and optimizer
    model = FastRCNN_ResNet(num_classes=1)
    optimizer = torch.optim.Adam(model.parameters(), lr=0.005, weight_decay=1e-5, momentum=0.9)

    # Train the model for 10 epochs
    model = train(model, train_loader, optimizer, num_epochs=10)

    # Evaluate the model
    mAP = evaluate(model, val_loader)
    print('mAP: {:.4f}'.format(mAP))



if __name__ == '__main__':
    main()


## drone size: 66 by 47
## https://medium.com/nanonets/how-i-built-a-self-flying-drone-to-track-people-in-under-50-lines-of-code-7485de7f828e

##using ROI with new class 
## https://medium.com/analytics-vidhya/training-your-own-data-set-using-mask-r-cnn-for-detecting-multiple-classes-3960ada85079

## YOLO
#https://github.com/theAIGuysCode/YOLOv4-Cloud-Tutorial

## Image seperation
# cam 0 = 42x127
# cam 1 = 27x183
# cam 2 = 48x167
# cam 3 = 40x102
# total = 579