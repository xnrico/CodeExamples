import numpy as np
from torch.utils.data import Dataset
from PIL import Image
import torch
import os
import torch.nn as nn
import torch.nn.functional as F
import torchvision.transforms as transforms
import torchvision.transforms.functional as TF
import random

def transform_train():
    transform = transforms.Compose(
        [transforms.ToTensor(),
         transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])
    return transform


def transform_test():
    transform = transforms.Compose(
        [transforms.ToTensor(),
         transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])
    return transform


# Data loader for the image segmentation data
class ImageSegmentation(Dataset):
    def __init__(self, root, split, transform=None, device="cpu"):
        self.root = root
        self.split = split
        self.transform = transform
        self.device = device

        self.dir = os.path.join(root, split)
        self.camera_files = sorted(os.listdir(os.path.join(self.dir, "Camera")))
        if self.split != "Test":
            self.seg_files = sorted(os.listdir(os.path.join(self.dir, "Labels")))

    def __len__(self):
        return len(self.camera_files)

    def image_augmentation(self, img_mat, label_mat):
        to_pil = transforms.ToPILImage()
        # Random crop
        img_mat = to_pil(img_mat)
        label_mat = to_pil(label_mat)
 
        # Random horizontal flipping
        if random.random() > 0.5:
            img_mat = TF.hflip(img_mat)
            label_mat = TF.hflip(label_mat)

        # Random vertical flipping
        if random.random() > 0.5:
            img_mat = TF.vflip(img_mat)
            label_mat = TF.vflip(label_mat)

        return np.array(img_mat), np.array(label_mat)

    # Return indexed item in dataset
    def __getitem__(self, index):
        file_name = os.path.join(self.dir, "Camera", self.camera_files[index])
        img = Image.open(file_name)
        img_mat = np.copy(np.asarray(img)[:, :, :3])
        if self.split != "Test":
            labeled_img = Image.open(os.path.join(self.dir, "Labels", self.seg_files[index]))
            label_mat = np.copy(np.asarray(labeled_img)[:, :, :3])
        else:
            label_mat = np.zeros_like(img_mat)
        if self.split == "Train":
            img_mat, label_mat = self.image_augmentation(img_mat, label_mat)
        return self.transform(img_mat), torch.tensor(label_mat, device=self.device)

    def collate_fn(self, data):
        B = len(data)
        img_batch = torch.stack([data[i][0] for i in range(B)]).to(self.device)
        label_batch = torch.stack([data[i][1] for i in range(B)]).to(self.device)
        return img_batch, label_batch


class ConvBlockStudent(nn.Module):
    def __init__(self, c_in, c_out, ds=False):
        super().__init__()
        if ds:
          self.net = nn.Sequential(
              nn.Conv2d(c_in, c_out, 2, stride=2, padding=0),
              nn.ReLU(),
              nn.BatchNorm2d(c_out),
          )
        else:
          self.net = nn.Sequential(
              nn.Conv2d(c_in, c_out, 3, stride=1, padding=1),
              nn.ReLU(),
              nn.BatchNorm2d(c_out),
          )

    def forward(self, x):
        return self.net(x)

class UNetStudent(nn.Module):
    def __init__(self, num_classes):
        super().__init__()
        self.pre = ConvBlockStudent(3, 16)

        self.down1 = ConvBlockStudent(16, 32, ds=True)
        self.down2 = ConvBlockStudent(32, 64, ds=True)
        self.down3 = ConvBlockStudent(64, 128, ds=True)

        self.up2 = ConvBlockStudent(128+64, 64, ds=True)
        self.up1 = ConvBlockStudent(64+32, 32)
        self.up0 = ConvBlockStudent(32+16, 32)

        self.out = nn.Conv2d(32, num_classes, kernel_size=3, stride=1, padding=1)

    def forward(self, x):
        # Encoder
        x0 = self.pre(x)
        x1 = self.down1(x0)
        x2 = self.down2(x1)
        x3 = self.down3(x2)

        # Going up 2nd layer
        B, __, H, W = x2.shape
        x3 = F.interpolate(x3, (H, W))
        x3 = torch.cat([x2, x3], dim=1)
        x2 = self.up2(x3)

        # Going up 1st layer
        B, __, H, W = x1.shape
        x2 = F.interpolate(x2, (H, W))
        x2 = torch.cat([x1, x2], dim=1)
        x1 = self.up1(x2)

        # Going up 0th layer
        B, __, H, W = x0.shape
        x1 = F.interpolate(x1, (H, W))
        x1 = torch.cat([x0, x1], dim=1)
        x = self.up0(x1)
        return self.out(x)



# Compute the per-class iou and miou
def IoU(targets, predictions, num_classes, ignore_index=0):
  intersections = torch.zeros(num_classes, device=targets.device)
  unions = torch.zeros_like(intersections)
  counts = torch.zeros_like(intersections)

  valid_mask = (targets != ignore_index)
  targets = targets[valid_mask]
  predictions = predictions[valid_mask]

  # Loop over classes and update the counts, unions, and intersections
  for c in range(num_classes):
    class_mask_target = (targets == c)
    class_mask_pred = (predictions == c)

    intersections[c] = torch.sum(class_mask_target & class_mask_pred).item()
    unions[c] = torch.sum(class_mask_target | class_mask_pred).item()
    counts[c] = torch.sum(class_mask_target).item()

    unions[c] = unions[c] + 0.00001

  # Per-class IoU
  # Make sure to set iou for classes with no points to 1
  iou = intersections / unions
  iou[counts == 0] = 1.0
  iou = torch.cat((iou[:ignore_index], iou[ignore_index + 1:]))

  # Calculate mean, ignoring ignore index
  miou = torch.sum(iou) / (num_classes - 1)
  return iou, miou