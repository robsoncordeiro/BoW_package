# The Method BoW for Clustering Terabyte-scale Datasets
    
<p>
    The method BoW focuses on the problem of finding clusters in Terabytes of moderate-to-high dimensionality data, such as features extracted from billions of complex data elements. In these cases, a serial processing strategy is usually impractical. Just to read a single Terabyte of data (at 5GB/min on a single modern eSATA disk) one takes more than 3 hours. BoW explores parallelism and can treat as plug-in almost any of the serial clustering methods. The major research challenges addressed are (a) how to minimize the I/O cost, taking care of the already existing data partition (e.g., on disks), and (b) how to minimize the network cost among processing nodes. Either of them may become the bottleneck. Our method automatically spots the bottleneck and chooses a good strategy, one of them uses a novel sampling-and-ignore idea to reduce the network traffic. Specifically, BoW combines (a) potentially any serial algorithm used as a plug-in and (b) makes the plug-in run efficiently in parallel, by adaptively balancing the cost for disk accesses and network accesses, which allows BoW to achieve a very good tradeoff between these two possible bottlenecks.
</p>
<p>
    <a href="http://doi.acm.org/10.1145/2020408.2020516"><b>Conference Publication</b></a><br/>
</p>

<h3>BoW - basic information</h3>

*******************************************************************

Input data format:
 
line_number axis_1 axis_2 axis_3 ... axis_d groundTruthCluster
line_number axis_1 axis_2 axis_3 ... axis_d groundTruthCluster
				.
				.
				.
line_number axis_1 axis_2 axis_3 ... axis_d groundTruthCluster				

Example input: Please contact Robson Cordeiro (robson@icmc.usp.br) to request one example dataset. Due to the large data size, the example is not included in this package.

Obs.: The groundTruthCluster data is not used by BoW.
      You may use this ground truth information to evaluate the quality of BoW's results.

*******************************************************************

Output data format:

The output is stored in the folder "BoW_package/results/$dataset_name/output_parallel_%r", where $dataset_name refers to the name of the input dataset and $r refers to the number of reducers used. The folder contains:

 - file "time": Contains the total running time in seconds.
 - files "part-*": these files describe the \beta-clusters found by each reducer. The minimum and maximum bounds of each \beta-cluster regarding each dimension are given to specify the hyper-rectangle that defines the \beta-cluster.
 - file "merded.dat": This file defines the final clustering result obtained after the merging stage. In the first line, it specifies the total number of clusters. Then, it labels each /beta-cluster found after one cluster. The order considered to label each \beta-cluster is as follows:

file "part-00000-sample": \beta-cluster 0 
file "part-00000-sample": \beta-cluster 1
file "part-00000-sample": \beta-cluster 2
           .
           .
           .
file "part-00000": \beta-cluster 0
file "part-00000": \beta-cluster 1
file "part-00000": \beta-cluster 2
           .
           .
           .
file "part-00001": \beta-cluster 0
file "part-00001": \beta-cluster 1
file "part-00001": \beta-cluster 2
           .
           .
           .
file "part-00002": \beta-cluster 0
file "part-00002": \beta-cluster 1
file "part-00002": \beta-cluster 2
           .
           .
           .

Notice that the file "part-00000-sample" exists only when our sampling-and-ignore idea was used. Nevertheless, the same order (starting at file "part-00000": \beta-cluster 0) also applies for the cases when our sampling-and-ignore idea was not used.

Example output: 

results/synthetic_hadoop_15d_100m.dat/output_parallel_256/

In this example, we provide the results obtained for our Synthetic dataset with 100 million points using 256 reducers. 10 clusters where found, as described in the file "merged.dat". Cluster 0 is defined by the \beta-clusters labeled with 0 in the file. Cluster 1 is defined by the \beta-clusters labeled with 1, and so on.

*******************************************************************
Configuring BoW:

The shell script file "BoW" includes the parameter configuration to be used by BoW. Please, set appropriate values (according to your environment) into the following sections in the beginning of the script:
 - system configuration
 - dataset configuration
 - cost estimation - environmental parameters
 - cost estimation - other parameters*

* Reasonable default values are provided for these parameters, which are expected to lead to high-quality clustering results for most cases.

*******************************************************************

Running BoW:

.\BoW

*******************************************************************
Plug-in information:

The plug-in serial clustering algorithm used here is the MrCC algorithm [Cordeiro et al., ICDE 2010, Cordeiro et al. TKDE-accepted], whose code is included in this package. Please, refer to the original papers for details regarding MrCC.

Notice however that BoW can use any serial clustering algorithm as a plug-in, provided that the plug-in describes clusters by hyper-rectangles. If you intend to use other serial algorithm as a plug-in for BoW, you will have to adapt two files of this package: "ParC/reducer.cpp" and "SnI/reducer.cpp". Both files read the data sent to a reducer and call MrCC to find clusters in the data - adapt them to call the other serial algorithm (instead of MrCC) and you'll be done.
