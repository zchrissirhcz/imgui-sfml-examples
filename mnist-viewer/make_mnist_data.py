from mnist import MNIST
import matplotlib.pyplot as plt
import numpy as np  # 导入numpy
import os

def save_images_and_labels(images, labels, directory, label_filename):
    """
    将图像和标签保存到指定目录。
    """
    if not os.path.exists(directory):
        os.makedirs(directory)
    
    label_file_path = os.path.join(directory, label_filename)
    with open(label_file_path, 'w') as label_file:
        for i, (image, label) in enumerate(zip(images, labels), start=1):
            image_filename = f"{i}.png"
            image_path = os.path.join(directory, image_filename)
            # 将图像数据转换为NumPy数组并重塑
            image_array = np.array(image, dtype=np.uint8).reshape(28, 28)
            # 保存图像
            plt.imsave(image_path, image_array, cmap='gray')
            # 写入标签
            label_file.write(f"{image_filename} {label}\n")

def main():
    # 加载MNIST数据集
    mndata = MNIST('mnist_data')
    train_images, train_labels = mndata.load_training()
    test_images, test_labels = mndata.load_testing()
    
    # 保存训练数据集
    save_images_and_labels(train_images, train_labels, 'train', 'train-label.txt')
    # 保存测试数据集
    save_images_and_labels(test_images, test_labels, 'test', 'test-label.txt')

if __name__ == "__main__":
    main()
