import os
import time
import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from net_class import Net, MyDataset
from PIL import Image
from sklearn.metrics import average_precision_score as ap_score
from torch.utils.data import Dataset, Subset, DataLoader, random_split
from torchvision import datasets, models, transforms
from tqdm import tqdm  

def train_epoch(model, dataLoader, criterion, optimizer, epoch):  # Train the model
        model.train()
        running_loss = []
        for i in enumerate(dataLoader):
            X = i[1]["img"].float()
            y = i[1]["label"].float()
            #initialize gradients to zero
            optimizer.zero_grad()

            #forward pass
            outputs = model(X)
            #compute loss
            loss = criterion(outputs, y)
            running_loss.append(loss.item())
            #backward step
            loss.backward()
            #step forward?
            optimizer.step()
            
        print("\n Epoch {} loss:{}".format(epoch+1, np.mean(running_loss)))
        with open("loss.txt","a") as f:
            print(np.mean(running_loss),file=f)
            print("")
        return running_loss, dataLoader


def evaluate(model, loader):  # Evaluate accuracy on validation / test set
    model.eval()  # Set the model to evaluation mode
    correct = 0
    with torch.no_grad():  # Do not calculate grident to speed up computation
        for i in enumerate(loader):
            batch = i[1]["img"].float()
            label = i[1]["label"].float()
            pred = model(batch)
            res = torch.sum(torch.sub(label, pred), dim=1)
            abs_floor = torch.floor(torch.abs(res))
            correct += (torch.numel(abs_floor[abs_floor<100]))
        acc = correct/len(loader.dataset)
        print("correct " + str(correct))
        print("Evaluation accuracy: {}".format(acc)) # CHANGE correct BACK TO acc !!
        return acc
    # Needs to be updated for a range, so that were not testing for 587.234234, but 587, need to convert res


def plot(train_acc, train_loss, val_acc, num_epochs):
    plt.figure()
    x = np.arange(num_epochs)
    plt.plot(x, train_acc)
    plt.plot(x, train_loss)
    plt.plot(x, val_acc)
    plt.legend(['Training Accuracy', 'Training Loss', 'Validation Accuracy'])
    plt.xticks(x)
    plt.xlabel('Epoch')
    plt.ylabel("accuracy or loss")
    plt.savefig("results.png", dpi = 300)

def test(testloader, net, criterion, device):
    '''
    Function for testing.
    '''
    losses = 0.
    cnt = 0
    with torch.no_grad():
        net = net.eval()
        for images, labels in tqdm(testloader):
            images = images.to(device)
            labels = labels.to(device)
            output = net(images)
            loss = criterion(output, labels)
            losses += loss.item()
            cnt += 1
    print('\n',losses / cnt)
    return (losses/cnt)

def main():
    #initialize train, validation, and test data_loaders

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

    # train_vals = Subset(data, range(2560))
    # valid_vals = Subset(data, range(2560,3200))
    # test_vals = Subset(data, range(3200,5334))
    
    #print(type(train_vals[0]['img']))
    print('created vals')

    train_loader = DataLoader(train_vals, batch_size, shuffle=True)
    val_loader = DataLoader(valid_vals, batch_size, shuffle=True)
    test_loader = DataLoader(test_vals, batch_size, shuffle=True)

    print('created loaders')
        
    #################################
    model = Net()
    criterion = nn.CrossEntropyLoss()

    # Parameters
    learning_rate = 1e-2
    weight_decay = 1e-5
    num_epoch = 30 

    optimizer = optim.Adam(model.parameters(), lr=learning_rate,
                    weight_decay=weight_decay) 
    
    epoch = 0
    train_loss_hist = []
    train_accu_hist = []
    val_accu_hist = []


    while epoch < num_epoch:
        # I think we need to remake this every time? before it seemed to be training and validating on the same thing every epoch

        running_loss, batch_data = train_epoch(model, train_loader, criterion, optimizer, epoch)
        
        #evaluate each epoch
        train_loss_hist.append(running_loss)
        print("training accuracy")
        train_accu_hist.append(evaluate(model, batch_data))
        print("\nvalidation accuracy")
        val_accu_hist.append(evaluate(model, val_loader))

        epoch += 1

    plot(train_accu_hist, train_loss_hist, val_accu_hist, num_epoch)

    print("test accuracy: " + str(evaluate(model, test_loader)))

if __name__ == '__main__':
    torch.manual_seed(442)
    np.random.seed(442)
    main()
    

# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMWNXXXXNNNNNNNNNNNNNNNNXXNWNKkc:kXNWNXNNNNNNNNNNNNNNNXXXXXXNWMMMMMMMMMMMMMMMWNXXXXNNNNNNNNNXNNNNNNXXNWNX0o;o0XWNNNNNNNNNNNNNNNNNNXXXXXNWMMMMMMMMMM
# MMMMMWK0OOkkxxdoollccccc::c::o0k:;;,;:x0o:c:::ccccclloodxxkkOO0KWMMMMMMMMMMMMMMMWX0OOkkxxddoollcccc:::::cOOc;;,;;l0x:cc:::ccccllooddxkkkOO0NMMMMMMMMMM
# MMMMMMMMMMMMWWNNXXKK0OOkxxddodKx,,,,,,d0xoddxkkO00KKXXNNWWMMMMMMMMMWWNNNNNWWWWMMMMMMMMMWWWNXXKK00Okkxxddo0O:,;,,,l0koddxxkOO0KKXXNNWWMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNd,,,,,,dNWWMMMMMMMMMWNNXXKK00OkkxxdoollcccclllodddxxkkO00KKXXNNWWMMMMMMMMWWO:,,,,,cKWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,,,,,dNWWWWWWWKxoollcc::;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;::cclloodkNWWWWWWO:,,,,,cKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMN0xol:,,,,,,:loooooooc,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;looloooc;,,,,,;lod0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMNx;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMM0:,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMXkxxxxxxxxxxxxxxxxxxxdl:;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;:loxxxxxxxxxxxxxxxxkxkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0ko:,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:ldk0XNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;dXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKl,,,,,;:c:::::::::;,,,;::c:::cc:::;,,,,,:OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKl,,,,,:kXXXXXXXXXX0c,,,lk0XXXXXXXXXOl,,,,,:OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl,,,,,l0WMMMMMMWMWWXl,,,oOXWWWWWMMMMWXd;,,,,:OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl,,,,;dXMMMMMMKdooool:;;,:cloooodKMMMMMNk:,,,,:OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl,,,,cOWMMMMMMMO;,,,;oO00Oxl:,,,,:OMMMMMMW0l;,,,cOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNo,,,,lKWMMMMMMMMO;,,;xN0ookNWO:,,,:OMMMMMMMMXd;,,,cKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO:,,,lKMMMMMMMMMMO;,,;xN0lcxNWO:,,,:OMMMMMMMMMNd;,,,xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWx,,,:OWMMMMMMMMMMO:,,,:dO000xoc;,,,:OMMMMMMMMMMKl,,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,,dNMMMMMMMMMMM0c:::::cllc:::::::l0MMMMMMMMMMWk;,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,;xWMMMMMMMMMMMWXXXXXXXXXXXXXXXXXXWMMMMMMMMMMM0:,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,;kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0:,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,;kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0c,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd,,;kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0c,,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;,c0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXo,,dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNKOKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX00NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
# MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM