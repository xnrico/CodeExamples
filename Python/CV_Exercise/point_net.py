from sys import _xoptions
import numpy as np
import torch
import os
import torch.nn as nn
from torch.utils.data import Dataset


class PointLoader(Dataset):
  def __init__(self, root, remap_array, data_split="Train",
              device="cpu", max_size=40000):
    self.root = root
    self.velo_dir = os.path.join(root, "velodyne_ds")
    self.velo_files = sorted(os.listdir(self.velo_dir))
    self.split = data_split
    if not self.split == "Test":
      self.label_dir = os.path.join(root, "labels_ds")
      self.label_files = sorted(os.listdir(self.label_dir))
    self.device = device
    self.remap_array = remap_array
    self.max_size = max_size
  
  def __len__(self):
    return len(self.velo_files)

  def __getitem__(self, index):
    # Load point cloud
    velo_file = os.path.join(self.velo_dir, self.velo_files[index])
    pc = np.fromfile(velo_file, dtype=np.float32).reshape(-1, 4)[:, 0:3]
    if self.split == "Test":
      return pc, None
    label_file = os.path.join(self.label_dir, self.label_files[index])
    label = np.fromfile(label_file, dtype=np.int32).reshape(-1) 
    label = label & 0xFFFF
    label = self.remap_array[label]
    
    # Downsample the points
    if self.split == "Train":
      indices = np.random.permutation(pc.shape[0])[:self.max_size]
      pc = pc[indices, :]
      label = label[indices]
    return pc, label

  def collate_fn(self, data):
    B = len(data)
    pc_numpy = [data[i][0] for i in range(B)]
    torch_pc = torch.tensor(np.array(pc_numpy), device=self.device)
    label_numpy = [data[i][1] for i in range(B)]
    torch_label = torch.tensor(np.array(label_numpy), device=self.device)
    return torch_pc, torch_label 


class PointNetEncoder(nn.Module):
  def __init__(self, cs, linear_out=None):
    super().__init__()
    self.net = torch.nn.Sequential()
    for i in range(1, len(cs)):
      self.net.add_module("Lin" + str(i), nn.Linear(cs[i-1], cs[i]))
      self.net.add_module("Bn" + str(i), nn.BatchNorm1d(cs[i]))
      self.net.add_module("ReLU" + str(i), nn.ReLU())
    if linear_out is not None:
      self.net.add_module("LinFinal", nn.Linear(cs[i], linear_out))

  def forward(self, x):
    B, N, C = x.shape
    x = x.view(B*N, C)
    x = self.net(x)
    x = x.view(B, N, -1)
    return x
    
class PointNetModule(nn.Module):
  def __init__(self, cs_en, cs_dec, num_classes=20):
    super().__init__()
    self.enc = PointNetEncoder(cs=cs_en)
    self.dec = PointNetEncoder(cs=cs_dec, linear_out=num_classes)

  def forward(self, x):
    B, N, C = x.shape
    # Encoder
    point_feats = self.enc(x)
    global_feats, _ = torch.max(point_feats, dim=1)
    global_feats = global_feats.view(B, 1, -1).repeat(1, N, 1)
    joint_feats = torch.cat([point_feats, global_feats], dim=2)
    out = self.dec(joint_feats)
    return out
    
# This module adds a T-Net
class PointNetFull(nn.Module):
  def __init__(self, cs_en, cs_dec, cs_t_en, cs_t_dec):
    super().__init__()
    self.t_enc = PointNetEncoder(cs=cs_t_en)
    self.t_dec = PointNetEncoder(cs=cs_t_dec, linear_out=9)
    self.joint_enc = PointNetModule(cs_en=cs_en, cs_dec=cs_dec)

  def forward(self, x):
    B, N, C = x.shape
    # T-Net
    t_feats = self.t_enc(x)
    t_feats, _ = torch.max(t_feats, dim=1)
    t_feats = t_feats.view(B, 1, -1)
    t_feats = self.t_dec(t_feats)
    point_trans = t_feats.view(B, 3, 3)
    point_trans = point_trans + torch.eye(3).view(1, 3, 3).expand(B, -1, -1).to(x.device)
    transformed_points = torch.bmm(x, point_trans)
    output_preds = self.joint_enc(transformed_points)
    return output_preds

class TestNet(nn.Module):
  def __init__(self, cs_en, cs_dec, cs_t_en, cs_t_dec, dropout_rate=0.3):
        super().__init__()
        self.pointNet = PointNetFull(cs_en=cs_en, cs_dec=cs_dec, cs_t_en=cs_t_en, cs_t_dec=cs_t_dec)
        # Adjust T-Net architecture
        self.t_feat_enc = PointNetEncoder(cs=cs_t_en)
        self.t_feat_dec = PointNetEncoder(cs=cs_t_dec, linear_out=9)
        # Introduce dropout for regularization
        self.dropout = nn.Dropout(dropout_rate)

  def forward(self, x):
      B, N, C = x.shape

      t_feat = self.t_feat_enc(x)
      t_feat, _ = torch.max(t_feat, dim=1)
      t_feat = t_feat.view(B, 1, -1)
      t_feat = self.t_feat_dec(t_feat)
      feat_trans = t_feat.view(B, 3, 3)
      feat_trans = feat_trans + torch.eye(3).view(1, 3, 3).expand(B, -1, -1).to(x.device)
      transformed_feats = torch.bmm(x, feat_trans)

      # Apply dropout for regularization
      transformed_feats = self.dropout(transformed_feats)

      output_preds = self.pointNet(transformed_feats)

      return output_preds


# Compute the per-class iou and miou
def IoU(targets, predictions, num_classes, ignore_index=0):
  intersections = torch.zeros(num_classes, device=targets.device)
  unions = torch.zeros_like(intersections)
  counts = torch.zeros_like(intersections)

  valid_mask = (targets != ignore_index)
  targets = targets[valid_mask]
  predictions = predictions[valid_mask]

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

    

