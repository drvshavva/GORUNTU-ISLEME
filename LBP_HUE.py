# -*- coding: utf-8 -*-
"""
Created on Wed Dec 12 05:59:50 2018

@author: havvanur
"""
import numpy as np
from matplotlib import pyplot as plt
import matplotlib.image as mpimg
import os
image_t = mpimg.imread("dne.jpg",0)
def load_images(folder):
    images = []
    for filename in os.listdir(folder):
        img = mpimg.imread(os.path.join(folder, filename))
        if img is not None:
            images.append(img)
    return images
images=load_images("C:/Users/havvanur/Desktop/train")
#ilk olarak rgb den luminance değerlerini çıkarıyoruz
def rgb2gray(rgb):
    r, g, b = rgb[:,:,0], rgb[:,:,1], rgb[:,:,2]
    gray = 0.299 *r + 0.587 *g + 0.114 *b
    return gray     
def rgb2hue(R,G,B):
     mx=max(R,G,B)
     mn=min(R,G,B)
     if (R==mx):
         hue=(G-B)/(mx-mn)
     elif(G==mx):
         hue=(2.0)+(B-R)/(mx-mn)
     else:
         hue=(4.0)+(R-G)/(mx-mn)
     return hue
def hue_histogram(seq,n_pixel):
    hist = {}
    for i in seq:
         hist[i] = hist.get(i, 0) + 1
    for i in seq:
        hist[i]/=n_pixel
    return hist

def lbp_histogram(seq,n_pixel):
    hist={}
    indis=[]
    uniform=[0, 1, 2, 3, 4, 6, 7, 8, 12, 14, 15, 16, 24, 28, 30, 31, 32, 48, 56, 60, 62, 63, 64, 96, 112, 120, 124, 126, 127, 128, 129, 131, 135, 143, 159, 191, 192, 193, 195, 199, 207, 223, 224, 225, 227, 231, 239, 240, 241, 243, 247, 248, 249, 251, 252, 253, 254 , 255]
    sm=0
    for i in seq:
        for j in range(len(uniform)):
            if(i==uniform[j]):
               hist[i] = hist.get(i, 0) + 1
               indis.append(i)
            else:
               sm+=i
    hist[59]=sm/n_pixel
    for i in indis:
        hist[i]/=n_pixel
    return hist

def pixel(img,center,x,y):
    new_value = 0
    try:
        if img[x][y] >= center:
            new_value = 1
    except:
        pass
    return new_value
def lbp(img,x,y):
    center=img[x][y]
    img_arr=[]
    val=0
    power_val = [1, 2, 4, 8, 16, 32, 64, 128]
    img_arr.append(pixel(img, center, x-1, y+1))     # sağ-üst
    img_arr.append(pixel(img, center, x, y+1))       # sağ
    img_arr.append(pixel(img, center, x+1, y+1))     # sağ alt
    img_arr.append(pixel(img, center, x+1, y))       # aşağısı
    img_arr.append(pixel(img, center, x+1, y-1))     # sol alt
    img_arr.append(pixel(img, center, x, y-1))       # sol
    img_arr.append(pixel(img, center, x-1, y-1))     # sol üst
    img_arr.append(pixel(img, center, x-1, y))       # üst
     
    for i in range(len(img_arr)):
        val += img_arr[i] *power_val[i]
            
    return val
def manhattan_distance(x,y):
    return min(sum(abs(a-b) for a,b in zip(x,y)))
def histogram(image):
    h_lbp=[]
    hues=[]
    img_lbp = np.zeros(( image.shape[0], image.shape[1],3),np.uint8)
    n_pixel=0
    gray = rgb2gray( image)
    for i in range(0, image.shape[0]):
        for j in range(0, image.shape[1]):
             [r, g, b] = image[i, j]
             r /= 255.0
             g /= 255.0
             b /= 255.0 
             hues.append(rgb2hue(r,g,b))
             h_lbp.append(lbp(gray, i, j))
             img_lbp[i, j]=lbp(gray, i, j)
             n_pixel+=1
     lbp_h=lbp_histogram(h_lbp,n_pixel)
     hue=hue_histogram(hues,n_pixel)
     plt.imshow(img_lbp, cmap = plt.get_cmap('gray'))
     plt.show()
        
return lbp_h,hue
lbp_test,h_test=histogram(image_t)
for i in range(0,len(images)):
    lbp_train,h_train=histogram(images[i])
    dist_h[i]=manhattan_distance(h_train,h_test)
    dist_lbp[i]=manhattan_distance(lbp_test,lbp_train)
    



