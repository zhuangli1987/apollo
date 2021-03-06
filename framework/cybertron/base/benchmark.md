## Benchmarks
这里给出了cybertron中各无锁数据结构在经典场景下，与stl数据结构加锁在多线程并发场景下性能对比的数据，由于并发环境下有很大的随机性，以下数据选取的是大量实验得出的典型值。

### AtomicRWLock benchmark
写优先模式下，同时起一个线程加写锁，多个线程加读锁

表1：加锁100次，处理过程1us的总耗时，单位ms

|ReadLocks    |1      |4      |8      |16       |32      |
|:------------|------:|------:|------:|--------:|-------:|
|AtomicRWLock |11.556 |12.745 |12.507 |21.327s  |72.869  |
|std::mutex   |11.865 |29.943 |54.007 |101.884s |196.195 |


### BoundedQueue benchmark
同时起相同个生产者和消费者线程，对shared_ptr&lt;int&gt;类型数据进行入队&出队操作

表2：各线程处理100,000个数据的总耗时，单位s

|c&p numbers  |1      |2      |4      |8      |16     |
|:------------|------:|------:|------:|------:|------:|
|BoundedQueue |0.0260 |0.0706 |0.1861 |0.8072 |1.8620 |
|std::queue   |0.0466 |0.1117 |0.3689 |1.0453 |3.2089 |

### AtomicHashMap benchmark
同时起多个线程对map进行插入、获取、删除三种操作，key为int类型，value为shared_ptr&lt;int&gt;类型，AtomicHashMap的表长度设置为1024。

表3：每个线程插入10000条数据耗时，单位ms

|Threads       | 1    | 2     | 4     | 8     | 16     |
|:-------------|-----:|------:|------:|------:|-------:|
|AtomicHashMap |7.036 |10.397 |17.327 |41.288 |61.565  |
|unordered_map |8.222 |20.612 |41.189 |80.149 |186.162 |

表4：每个线程获取10000条数据耗时，单位ms

|Threads       | 1    | 2     | 4     | 8     | 16     |
|:-------------|-----:|------:|------:|------:|-------:|
|AtomicHashMap |2.870 |4.484  |7.165  |12.613 |21.649  |
|unordered_map |3.994 |14.886 |21.484 |48.452 |126.884 |

表5：每个线程删除10000条数据耗时，单位ms

|Threads       | 1    | 2     | 4     | 8     | 16     |
|:-------------|-----:|------:|------:|------:|-------:|
|AtomicHashMap |3.786 |4.012  |5.812  |6.168  |21.265  |
|unordered_map |4.242 |12.721 |18.918 |54.154 |114.157 |
