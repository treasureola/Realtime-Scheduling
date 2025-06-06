# Design of Autonomous Systems | MP2

Instructor: [Prof. Sibin Mohan](https://sibin.github.io/sibin), The George Washington University

CSCI 4907/6907 Section 86 | Spring 25 | Tue/Thur 12:45PM - 02:00PM ET | TOMP 310


## Administrivia
| Syntax | Description |
| --- | ----------- |
| Announcement Date | Feb. 17, 2025 |
| Submission Date	| Mar. 03, 2025 |
| Submission Time	| 6:00 PM ET |
| Total Points	 | 15 |


## MP2 | Real-Time Schedulers

In this MP, you will implement scheduling simulators for the two most common _single core_ real-time scheduling algorithms [^1]:

1. Rate-Monotone (RM), a fixed-priority scheduler and
2. Earliest Deadline First (EDF), a dynamic priority scheduler 

## Table of Contents
- [Definitions and Theory](#definitions-and-theory)
- [References](#references)
- [Objective](#objective)
- [Submission](#submission-and-gradin-rubric)

## Definitions and Theory
A real-time task, $\tau_i$ is defined using the following parameters: $(\phi_i, p_i, c_i, d_i)$ where,

| Symbol | Description |
| ------ | ----------- |
| $\phi_i$ | Phase (offset for the first job of a task) |
| $p_i$    | Period |
| $c_i$    | Worst-case execution time |
| $d_i$    | Deadline |

Hence, a real-time tast _set_ (of size '_n_') is collection of such tasks, _i.e.,_ $\tau = {\tau_1, \tau_2, ... \tau_n}$. 

Given a real-time task set, the _first_ step is to check if the task set is **schedulable**, _i.e.,_ check whether all jobs of a task will meet their deadlines. For this purpose, multiple **schedulability tests** have been developed, each depending on the scheduling algorithm being used.

<details>
<summary>[Hints]</summary>

> - remember that task is a set of parameters.
> - We "release" multiple "_jobs_" of each task, each with its own deadline
> - if all jobs of all tasks meet their deadlines, then the system remains _safe_.
</details>

### Schedulability Analyses

As mentioned earlier, the exact schedulability test depends on the scheduling algorithm being used. In this MP you will develop two scheduling algorithms, RM and EDF, as mentioned earlier. They have subtly different schedulability tests [^2], based on system **utilization**. 

Utilization is the fraction of the processor/CPU that is consumed by all the jobs of all the tasks in the system. For any given task, $tau_i$, the utilization of _all_ of its jobs is: 

$$
U_i = \frac{C_i}{P_i}
$$

Hence, the _total_ utilization for all the tasks in the system is,

$$
U = U_1 + U_2 ... U_n = \frac{C_1}{P_1} + \frac{C_2}{P_2} + ... \frac{C_n}{P_n}
$$

_i.e.,_ utilization, 

$$
U = \sum_{i=1}^{n}\frac{C_i}{P_i}
$$

#### Utilization Tests

a. The **utilization bound** for $n$ periodic tasks in **Rate Monotonic Scheduling** is defined as,

$$
UB_n = n.(2^{1/n} - 1)
$$

Hence, a task set is **mostly** schedulable under the RM algorithm if, 

$$
U < UB_n
$$

**Always** check the response-time analysis bound for RM, _i.e.,_

$$ R_i = c_i + \sum_{j\in hp(i)}\left\lceil \frac{R_i}{T_j} \right\rceil .\ c_j $$

For each task, we carry out the above analysis &rarr; stop when consecutive iterations provide the **same** values. At each stage, check if the response time for a task is less than or equal to its deadline

$$ \forall \tau_i: R_i < T_i $$

b. A task set is schedulable under the **Earliest Deadlines First Scheduling** if,

$$
U \le 1
$$

**Note**: recall the policies for when, 

- tasks have same periods (RM) or deadlines (EDF)

## Objective

The main objective of this MP is to write simulators that implement both, the RM and EDF algorithms. Given a task set, $\tau$ and parameters of its constituent tasks, you are expected to:

1. check the schedulability of the task set and
2. simulate, for **one hyperperiod**, the execution pattern of the task set

<details>
<summary>[Hints]</summary>

> - remember that a hypeperiod is the LCM of the periods of all the tasks in the task set
</details>


You can write your code in any programming language you prefer but it **must** compile down+ to an executable named, `sched`. The program will take the following _command line_ arguments:

| command line flag | description | parameters |
|-------------------|-------------|------------|
| `-i` | input file name with task list | a text file (see below for format) |
| `-e` | which scheduling algorithm to simulate | one of `rm` or `edf` |
| `-o` | output file name | a text file (see below for format) |

For instance, if we want to simulate RM on an input file, `input.txt` and write the output to `output.txt`, 

```./sched -i input.txt -e rm -o output.txt```

**Note:** 

- the order of flags is **not** guaranteed and
- +you must include a Makefile that generates an **executable** named `sched`

### File Formats

1. **Input file format** will follow the following template:

```
<task 1 name, phase, period, worst-case execution time, deadline> (comma separated on one line)
<task 2 name, phase, period, worst-case execution time, deadline>
...
```

Here is an example ([taskset.1.txt](./tasksets/public_tasksets/taskset.1.txt)):
```
T1, 0, 4, 1, 4
T2, 0, 6, 2, 6
T3, 0, 8, 2, 8
```

2. **Output File Format**: you must follow this template **EXACTLY** otherwise the grading scripts will break and you will lose points.

```
<Scheduling algorithm Name -- one of RM or EDF>
One of <SCHEDULABLE/NOT SCHEDULABLE>
U = <utilization up to THREE decimal places>
UB = <computed utilization bound (up to THREE decimal places) for RM, "1.0" for EDF>
HYPERPERIOD = <hyperperiod/lcm of all task periods>
<EMPTY LINE>
<Time Unit> <Task Name or BLANK if empty> <DEADLINE_MISS(Taskname) if any>
```

For the above example, and for **RM**, the [output](./tasksets/public_tasksets/rm.tastkset.1.txt) will be:
```
RM
SCHEDULABLE
U = 0.833
UB = 0.780
HYPERPERIOD = 24 

0 T1 
1 T2 
2 T2 
3 T3 
4 T1 
5 T3 
6 T2 
7 T2 
8 T1 
9 T3 
10 T3 
11 
12 T1 
13 T2 
14 T2 
15 
16 T1 
17 T3 
18 T2 
19 T2 
20 T1 
21 T3 
22 
23 
```

The same example, under **EDF**, will print the following [output](./tasksets/public_tasksets/edf.tastkset.1.txt),
```
EDF
SCHEDULABLE
U = 0.833
UB = 1.0
HYPERPERIOD = 24 

0 T1 
1 T2 
2 T2 
3 T3 
4 T3 
5 T1 
6 T2 
7 T2 
8 T1 
9 T3 
10 T3 
11 
12 T1 
13 T2 
14 T2 
15 
16 T1 
17 T3 
18 T3 
19 T2 
20 T2 
21 T1 
22 
23 
```

**Note:** the above output must be written to the output file name specified in the command line parameters.

**Deadline Misses:**

1. must be printed in the **same** cycle as they occur. For instance, in [taskset.2.txt](./tasksets/public_tasksets/taskset.2.txt), the first job of `T3` has the $D = 7$ and the job is only able to finish after `7` cycles (recall that the timeline starts at `0`). So, print the deadline miss in the $7^{th}$ cycle as shown below.

```
RM
NOT SCHEDULABLE
U = 0.936
UB = 0.780
HYPERPERIOD = 140 

0 T1 
1 T2 
2 T2 
3 T3 
4 T1 
5 T2 
6 T2 
7 T3  DEADLINE_MISS(T3) 
```

2. if **multiple jobs** miss their deadlines, then print them on the same line, with a space between the, **ordered by task name**, _e.g._, for some fictional task set,

```
21 T2 DEADLINE_MISS(T4) DEADLINE_MISS(T5)
```


## Submission and Grading Rubric

You **must** submit the following:
1. _all_ of your code
2. Makefile
3. All the test cases (input files/task sets) with corresponding output files -- place it in a separate subfolder named `tests` and name your input/output files correspondingly, _e.g.,_ `input1.txt` and `output1.txt`

| Task | Points |
| --- | --- |
| Code compiles (2), creates an executable named `sched` (1) runs to completion (2),  | 5 points |
| Passes the **two** publicly provided test cases [see the [tasksets/public_tasksets](./tasksets/public_tasksets/) folder] | 2 points |
| Passes the **four** hidden test cases | 8 Points |

**Note:** if your code doesn't compile then you get `0` for the entire assignment. 

## References

[^1]: "_[Real time scheduling theory: A historical perspective](https://www.iris.sssup.it/bitstream/11382/303032/1/rtsj04-edf.pdf)_" by Lui Sha, Tarek Abdelzaher, Karl-Erik Årzén, Anton Cervin, Theodore Baker, Alan Burns, Giorgio Buttazzo, Marco Caccamo, John Lehoczky, and Aloysius K. Mok. Real-time systems 28 (2004): 101-155.

[^2]: "_[Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment](https://dl.acm.org/doi/pdf/10.1145/321738.321743)_" by C. L. Liu and James W. Layland. J. ACM 20, 1 (Jan. 1973), 46–61. https://doi.org/10.1145/321738.321743
