# POROTO Usage in conjunction with OmpSs

Poroto allows to guide C to VHDL compilation using ROCCC and handles the
generation of glue logic interfacing the FPGA kernel to memory blocs.

Poroto provides a software wrapper enabling to mange the execution of  the
offloaded function from the CPU.

The use of Poroto kernel along with the OmpSs programming model, allows the
user to parallelise execution by  exploiting the FPGA resources amde available
through Poroto managed designs.

The use of Poroto along with OmpSs was is explored in the examples provided in
this folder

## Installation pre-requisites

To explore capabilities of combined usage of Poroto and OmpSs, user needs to
install the later.
For this end, please refer  refer to [Installation of OmpSs](https://pm.bsc.es/ompss-docs/user-guide/installation.html).

**IMPORTANT REMARK:** Both Poroto and OmpSs instantiate an environment variable
called TARGET which may generate conflicts and inconsistent behaviour.
If OmpSs is not yet installed, the user should give another name to this global
variable along the installation process. For example change:
```bash
  export TARGET=$HOME/ompss
```
to
```bash
  export OMPSSTARGET=$HOME/ompss
  ```
User should adapt consequently all the occurrences in `./configure` and export commands during installation.

If OmpSs is already installed, user should edit the `makefile` template in Poroto to replace the `‘TARGET’` occurrences using another name such as `POROTO_TARGET` or similar.

## Examples

### Matrix multiplication

This is the first example used to evaluate the way a developer would proceed to adapt his code, on step-by-step optimisation.
In particular, by exploiting the CPU/FPGA interaction through OmpSs programming model in conjunction with FPGA wrappers available from Poroto, the examples provided show differents ways of adapting and annotating the code that result in various performance results.

The examples provided were tested on an FPGA enabled machine (in our case an 8-core CPU machine with an FPGA accelerator board from AlphaData )

The design time process of exploring and optimizing the achievable performance of the example computation on the target machine, follows the following steps:

First the developer uses Poroto tool to annotate and offload the code of matrix multiplication function on the FPGA.
This step results in the generation of a kernel that the user can characterize through a CPU testbench that executes the wrapper function for the FPGA kernel provided by Poroto.

The user can, at this step, compare the performance of the offloaded function to its initial version on the CPU.

In our example, this results into an FPGA execution roughly 5 times slower that the CPU one.

The developer may then choose to manually rewrite his code to statically run one FPGA execution every five matrix multiplications on the CPU.

However, unsurprisingly, if no explicit parallelism is inferred, the total execution time is roughly equal to the sum of the execution times for CPU and FPGA and gets then significantly longer than the one for CPU-Only execution.

To infer explicit parallelism, we use OmpSs and try to annotate the tasks in a way that result in better execution time and resource usage.

In the various sub-examples provided, various annotations are evaluated starting from the modified code with the pre-set distribution between executions on FPGA and CPU.

Depending on the way the code is annotated, the dependencies generated by OmpSs may result in various loads on the resources (i.e. the CPU cores and the FPGA). Besides, from OmpSs perspective, **the annotated tasks are treated as normal CPU task even for the ones that execute the wrapper function for the FPGA kernel**.

Successive variations of the annotated code have shown better optimization at each step  until performing better than the CPU-only parallel version. This was achieved for the statically and explicitly fixed repartition of 1/5th of computations on FPGA (FPGABenchAllPar3 example).

The analysis of the execution diagrams (using *Paraver/Extrae* tools) shows that there is still room for optimization. The modified code in FPGABenchAllPar4 and FPGABenchAllPar5), by adding a synchronization flag inside the task annotations and code in a way that we could intercept the end of an FPGA execution as soon as it is achieved. That way, it is possible to launch as soon as possible the next execution from a different task running even from another core.

The resulting code and annotation represents a basic pattern to exploit optimally a Poroto generated FPGA kernel wrapper with OmpSs, without requiring OmpSs being aware of the FPGA resource itself, neither having the user operating a manual repartition of the computations to be run on FPGA and/or CPU.

Here below the code snippet for the most elaborated example (FPGABenchAllPar5) :

```c
for (i = 0; i<100; i++) {
#pragma omp target device(smp) copy_deps
#pragma omp task in (A_1, A_2, B_1, B_2) out(C_1, C_2, flag, iter_fpga, iter_cpu)
{
#pragma omp task in (A_1, A_2, B_1, B_2) out(C_1, C_2, flag, iter_fpga, iter_cpu)
      {
if (flag)
        {
flag = 0;
FPGAMatrixMultiplication((int **) A_2, (int **) B_2, height_A, width_A_height_B, width_B, (int **) C_2);
          ...
iter_fpga++;
flag = 1;
        }
else
        {
          C_1 = CPUMatrixMultiplication(A_1, B_1, height_A, width_B, width_A_height_B);
          ...
        }
      }
/*this task is just to count of the iterations on cpu and to suppress the dependency*/
#pragma omp task in (flag) out(iter_cpu)
if (!flag) iter_cpu++;
  }
}
#pragma omptaskwait
```