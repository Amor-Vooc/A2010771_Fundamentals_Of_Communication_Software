# CQUPT 重庆邮电大学 A2010771 通信软件基础
## Visual Studio 2022 环境下正常运行
## 本课程项目已全部完成！

## task_1_Signal_Analyze

程序通过读取实际信号和确定信号文件，计算和排序实际信号的功率值，进一步对选定的实际信号和确定信号进行相关性检测，最终输出相关性结果。程序的设计旨在高效地处理多个信号文件，并通过多线程优化性能，以便在较短的时间内完成大量的数据处理和分析任务。

## task_2_Clustering_Algorithms

该程序通过实现K-Means聚类算法，对15个国家在三个体育赛事中的表现数据进行归类分析。通过数据归一化、簇中心初始化、迭代分配与更新，最终实现数据的有效分类，并输出详细的聚类过程和结果。

## task_3_BMPRotation

程序实现了对24位色深BMP格式图像的旋转功能。通过读取BMP文件，利用数学变换实现图像的旋转，并通过插值算法处理图像质量，最终输出旋转后的图像。

## task_4_SOCKET_TCP_Server&Client

程序实现了一个基于 TCP 协议的 多人聊天室系统，分为 服务器端 和 客户端。
* 服务器端代码用于创建和管理聊天室服务，包括接收客户端的连接、转发消息、管理用户状态等功能，且让客户端可以自定义用户名。
* 客户端代码用于连接服务器、发送/接收消息、以及与其他用户交互，支持自定义消息颜色。

## task_5_Link_State_Routing_Protocol

该程序模拟了一个小型路由网络（包含6个路由器），通过加载邻接矩阵表示的网络拓扑结构，
 
1. 加载路由器间的连接数据： 
* 从文件中读取每个路由器的连接情况，并进行洪泛，构建邻接矩阵。 
* 支持基本的错误处理，例如文件不存在或数据非法。 

2. 计算最短路径： 

* 使用 Dijkstra 算法计算每个路由器的路由表（包括目标路由的距离和下一跳信息）。

3. 展示网络信息： 

* 输出路由器的 IP 地址及其邻接矩阵。 
* 显示每个路由器的路由表。 

4. 模拟路由间数据传输： 

* 根据计算出的路由表，模拟从一个路由器向另一个路由器传输数据，并展示路径。