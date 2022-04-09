# ReservationTable

Class objects are used as reservation table for [```Router```](router.md)s.

### Method
```c++
void Reserve(Connection dest_in, Connection dest_out)
```
Reserves the given input to the given output

### Method
```c++
void Release(Connection dest_in)
```
Removes bindings for the given input

### Method
```c++
bool Reserved(Connection dest_in, Connection dest_out) const
```
Returns true if the given input is bound to the given output

### Method
```c++
bool Reserved(Connection dest_out) const
```
Returns true if there is bound [```Connection```](connection.md) instance to the given output

### Method
```c++
Connection operator[](Connection dest_in) const
```
Returns instance of the [```Connection```](connection.md) bound to the given input.
If there is no bound [```Connection```](connection.md), it returns invalid instance.

### Method
```c++
friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table)
```
Overloaded output operator to print formatted [```ReservationTable```](reservation_table.md) data