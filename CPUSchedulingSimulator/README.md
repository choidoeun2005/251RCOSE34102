# CPU Scheduling Simulator

## v 1.0

## Demo Video Link
- https://youtu.be/yYbmWmn6nvc

## Functional Updates

- Completed RR algorithm
- Revised CLI

## How To Use

  <table>
    <tr bgcolor="#e3e3e3" style="text-align: center">
      <td scope="col"><b>command</b></td>
      <td scope="col"><b>subcommand</b></td>
      <td scope="col"><b>arguments</b></td>
      <td scope="col"><b>options</b></td>
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
      <td style="text-align: center">&lt;PID&gt;</td>
      <td style="text-align: center">-</td>
      <td>Removes the specified process from memory.</td>
    </tr>
    <tr>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-a</td>
      <td>Removes all processes from the memory.</td>
    </tr>
    <tr>
      <td rowspan="3" style="text-align: center">run</td>
      <td style="text-align: center">-</td>
      <td style="text-align: center">&lt;FCFS|SJF|PRIORITY&gt;</td>
      <td style="text-align: center">[-p]</td>
      <td>Runs the CPU scheduler with the chosen algorithm. -p enables preemptive mode.</td>
    </tr>
    <tr>
      <td style="text-align: center"></td>
      <td style="text-align: center">RR,  [time_quantum]</td>
      <td style="text-align: center">-</td>
      <td>Runs the CPU scheduler with the Round Robin algorithm with the specified time quantum (default 10).</td>
    </tr>
    <tr>
      <td style="text-align: center">-</td>
      <td style="text-align: center"></td>
      <td style="text-align: center">-a</td>
      <td>Runs the CPU scheduler with all possible algorithms (FCFS, SJF, PRIORITY, RR). Runs the preemtive mode as well if possible.</td>
    </tr>
    <tr>
      <td rowspan="5" style="text-align: center">compare</td>
      <td rowspan="3" style="text-align: center">add</td>
      <td style="text-align: center">&lt;FCFS|SJF|PRIORITY&gt;</td>
      <td style="text-align: center">[-p]</td>
      <td>Adds or removes an algorithm to/from the compare view. [-p] option to enable preemtive mode (if possible).</td>
    </tr>
    <tr>
      <td style="text-align: center">RR,  [time_quantum]</td>
      <td style="text-align: center"></td>
      <td>Adds removes Round Robin algorithm to the compare view. Time quantum can be set by the argument. (Default 10)</td>
    </tr>
    <tr>
      <td style="text-align: center">-</td>
      <td style="text-align: center">-a</td>
      <td>Adds all algorithms to the compare view. Round Robin is added with the default time quantum 10.</td>
    </tr>
    <tr>
      <td style="text-align: center">clear</td>
      <td colspan="2" style="text-align: center">-</td>
      <td>Removes all algorithms from the compare view.</td>
    </tr>
    <tr>
      <td colspan="3" style="text-align: center">-</td>
      <td>Displays compare view (up to 10 algorithms).</td>
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

## Notes
- All commands are case-insensitive.
  ('run sjf' has the same effect as 'run SJF')
- Lower numbers indicate higher priority.

## Mandatory

- <b> FCFS (Completed) </b>

- <b> SJF (Completed) </b>
- <b> Priority (Completed) </b>
- <b> Preemtive SJF (Completed) </b>
- <b> Preemtive Priority (Completed) </b>
- <b> Round Robin (Completed) </b>

## Optional

- Multilevel Queue Scheduling (TODO)
- Decaying (TODO)
