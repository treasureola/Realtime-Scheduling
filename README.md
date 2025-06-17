## Real-Time Schedulers

In this MP, you will implement scheduling simulators for the two most common _single core_ real-time scheduling algorithms [^1]:

1. Rate-Monotone (RM), a fixed-priority scheduler and
2. Earliest Deadline First (EDF), a dynamic priority scheduler 

## Table of Contents
- [Definitions and Theory](#definitions-and-theory)
- [References](#references)
- [Objective](#objective)

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

The main objective of this project is to write simulators that implement both, the RM and EDF algorithms. Given a task set, $\tau$ and parameters of its constituent tasks, you are expected to:

1. check the schedulability of the task set and
2. simulate, for **one hyperperiod**, the execution pattern of the task set

<details>
<summary>[Hints]</summary>

> - remember that a hypeperiod is the LCM of the periods of all the tasks in the task set
</details>

## References

[^1]: "_[Real time scheduling theory: A historical perspective](https://www.iris.sssup.it/bitstream/11382/303032/1/rtsj04-edf.pdf)_" by Lui Sha, Tarek Abdelzaher, Karl-Erik Årzén, Anton Cervin, Theodore Baker, Alan Burns, Giorgio Buttazzo, Marco Caccamo, John Lehoczky, and Aloysius K. Mok. Real-time systems 28 (2004): 101-155.

[^2]: "_[Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment](https://dl.acm.org/doi/pdf/10.1145/321738.321743)_" by C. L. Liu and James W. Layland. J. ACM 20, 1 (Jan. 1973), 46–61. https://doi.org/10.1145/321738.321743
