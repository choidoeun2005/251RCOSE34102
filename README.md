# CPU Scheduling Simulator

## v 0.2

## Functional Updates

- Implemented CLI
- Reformed Gantt Chart interface

## How To Use

  <table>
    <tr bgcolor="#e3e3e3" style="text-align: center">
      <td scope="col"><b>command</b></td>
      <td scope="col"><b>subcommand</b></td>
      <td scope="col"><b>options</b></td>
      <td scope="col"><b>arguments</b></td>
      <td scope="col"><b>description</b></td>
    </tr>
    <tr>
      <td style="text-align: center">add</td>
      <td colspan="3" style="text-align: center">-</td>
      <td>Adds a new process to memory through an interactive dialogue sequence. Type -1 any time to suspend.</td>
    </tr>
    <tr>
      <td rowspan="2" style="text-align: center">remove</td>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-</td>
      <td style="text-align: center">&lt;PID&gt;</td>
      <td>Removes the specified process from memory.</td>
    </tr>
    <tr>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-a</td>
      <td style="text-align: center">-</td>
      <td>Removes all processes from the memory.</td>
    </tr>
    <tr>
      <td rowspan="3" style="text-align: center">run</td>
      <td style="text-align: center">&lt;FCFS|SJF|PRIORITY&gt;</td>
      <td style="text-align: center">[-p]</td>
      <td style="text-align: center">-</td>
      <td>Runs the CPU scheduler with the chosen algorithm. -p enables preemptive mode.</td>
    </tr>
    <tr>
      <td style="text-align: center">RR</td>
      <td style="text-align: center">-</td>
      <td style="text-align: center">&lt;time_quantum&gt;</td>
      <td>Runs the CPU scheduler with the Round Robin algorithm with the specified time quantum.</td>
    </tr>
    <tr>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-a</td>
      <td style="text-align: center">-</td>
      <td>Runs the CPU scheduler with all possible algorithms (FCFS, SJF, PRIORITY, RR). Runs the preemtive mode as well if possible.</td>
    </tr>
    <tr>
      <td style="text-align: center">list</td>
      <td colspan="3" style="text-align: center">-</td>
      <td>Show all the processes in memory.</td>
    </tr>
    <tr>
      <td style="text-align: center">exit</td>
      <td colspan="3" style="text-align: center">-</td>
      <td>Exit the CPU scheduler.</td>
    </tr>
    <tr>
      <td style="text-align: center">help</td>
      <td colspan="3" style="text-align: center">-</td>
      <td>Displays command list.</td>
    </tr>
  </table>

## Mandatory

- <b> FCFS (Completed) </b>

- <b> SJF (Completed) </b>
- <b> Priority (Completed) </b>
- Preemtive FCFS (TODO)
- Preemtive SJF (TODO)
- Preemtive Priority (TODO)
- Round Robin (TODO)

## Optional

- Multilevel Queue Scheduling (TODO)
- Decaying.. (TODO)
