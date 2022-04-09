# FlitType

Enum representing the type of flit. It is used as flag, so each flit can have several types.

### Possible values
- ```None``` - should be never used
- ```Head``` - leading flit of the packet
- ```Body``` - body (or data) flit of the packet
- ```Tail``` - closing flit of the packet