# coding=gbk
import sys
import matplotlib.pyplot as plt
import os

def plot_file(filename):
    # ��ȡ�ļ��е�yֵ�������зָ�
    with open(filename, 'r') as file:
        y_values = [float(line.strip()) for line in file.readlines()]
    
    # ����xֵΪ����
    x_values = list(range(1, len(y_values) + 1))
    
    # ��ȡy�����ֵ��Ϊy_values���ֵ��80%
    max_y_value = max(y_values) * 1.25
    
    # ����һ����ͼ��
    plt.figure(figsize=(20, 10))

    # ��������ͼ
    plt.plot(x_values, y_values, marker='o', linestyle='-', color='b', markersize=0)
    
    # ����X��ķ�Χ����1��������
    plt.xlim(1, len(x_values))
    
    # ����Y��ķ�Χ����0�����yֵ��1.25��������80%������
    plt.ylim(0, max_y_value)
    
    # ����ͱ�ǩ
    plt.title(f"Line Plot for {filename}")
    plt.xlabel("X (Line number)")
    plt.ylabel("Y (Value)")
    
    # ��ʾͼ��
    plt.grid(True)
    plt.show()

def main():
    # ������������ȡ�ļ���
    if len(sys.argv) < 2:
        print("\033[31m[ERR]\033[0m\tȱ�ٻ�ͼ�������ļ�������")
        sys.exit(1)

    # �������д�����ļ�
    for filename in sys.argv[1:]:
        if os.path.exists(filename):
            print(f"\033[32m[INFO]\033[0m\tPlotting file: {filename}")
            plot_file(filename)
        else:
            print(f"\033[33m[WARN]\033[0m\tFile not found: {filename}")

if __name__ == "__main__":
    main()
