# ProgressBar

Class object is used to print current progress of the simulation

### Method
```c++
void Update()
```
Updates current progress and prints it to the output stream

### Constructor
```c++
ProgressBar(sc_module_name, std::ostream& os, const SimulationTimer& network_timer, std::int32_t units, const sc_clock& clk);
```
Creates new instance of progress bar and binds ```Update``` method to clock signal