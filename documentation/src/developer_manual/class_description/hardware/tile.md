# Tile

Storage class representing couple of [```Processor```](processor.md) and [```Router```](router.md)

### Field
```c++
sc_in_clk clock
```
Clock signal

### Field
```c++
sc_in<bool> reset
```
Reset signal

### Field
```c++
std::unique_ptr<Router> RouterDevice
```
Unique pointer to [```Router```](router.md) instance

### Field
```c++
std::unique_ptr<Processor> ProcessorDevice
```
Unique pointer to [```Processor```](processor.md) instance


### Method
```c++
void SetRouter(std::unique_ptr<Router>& router)
```
Assigns [```Router```](router.md) instance

### Method
```c++
void SetProcessor(std::unique_ptr<Processor>& processor)
```
Assigns [```Processor```](processor.md) instance