# coding=gbk
import sys
import matplotlib.pyplot as plt
import os

def plot_file(filename):
    # 读取文件中的y值，按换行分隔
    with open(filename, 'r') as file:
        y_values = [float(line.strip()) for line in file.readlines()]
    
    # 设置x值为行数
    x_values = list(range(1, len(y_values) + 1))
    
    # 获取y轴最大值，为y_values最大值的80%
    max_y_value = max(y_values) * 1.25
    
    # 创建一个新图形
    plt.figure(figsize=(20, 10))

    # 绘制折线图
    plt.plot(x_values, y_values, marker='o', linestyle='-', color='b', markersize=0)
    
    # 设置X轴的范围：从1到总行数
    plt.xlim(1, len(x_values))
    
    # 设置Y轴的范围：从0到最大y值的1.25倍（保持80%比例）
    plt.ylim(0, max_y_value)
    
    # 标题和标签
    plt.title(f"Line Plot for {filename}")
    plt.xlabel("X (Line number)")
    plt.ylabel("Y (Value)")
    
    # 显示图形
    plt.grid(True)
    plt.show()

def main():
    # 从启动参数读取文件名
    if len(sys.argv) < 2:
        print("\033[31m[ERR]\033[0m\t缺少绘图参数的文件名参数")
        sys.exit(1)

    # 遍历所有传入的文件
    for filename in sys.argv[1:]:
        if os.path.exists(filename):
            print(f"\033[32m[INFO]\033[0m\tPlotting file: {filename}")
            plot_file(filename)
        else:
            print(f"\033[33m[WARN]\033[0m\tFile not found: {filename}")

if __name__ == "__main__":
    main()
