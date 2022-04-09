# Configuration

```Configuration``` class object is used as a container of the simulation parameters.
It is passed throug most other classes to provide them access to rules of their behaviour.

### Constructor 
```c++
Configuration(std::int32_t arg_num, char* arg_vet[])
```
Accepts command line arguments number and array of strings, representing passed arguments.
Used to initialize whole simulator configuration from command line arguments and configuration file.

### Method 
```c++ 
void ParseArgs(YAML::Node &node, std::int32_t arg_num, char* arg_vet[])
```
- Accepts root yaml node, command line arguments number and array of strings, representing passed arguments.
- Used to override existing configuration, initialized from file with command line arguments.

### Method 
```c++
void ReadTopologyParams(const YAML::Node& config)
```
- Accepts root yaml node
- Used to read topology parameters from yaml root node.

### Method
```c++ 
void ReadRouterParams(const YAML::Node& config)
```
- Accepts root yaml node
- Used to read router parameters from yaml root node.

### Method
```c++
void ReadRoutingTableParams(const YAML::Node& config)
```
- Accepts root yaml node
- Used to read routing table parameters from yaml root node.

### Method 
```c++
void ReadSimulationParams(const YAML::Node& config)
```
- Accepts root yaml node
- Used to read simulation parameters from yaml root node.

### Method 
```c++
void ReadTrafficDistributionParams(const YAML::Node& config)
```
- Accepts root yaml node
- Used to read traffic distribution parameters from yaml root node.
