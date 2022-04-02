# Routers configuration



#### 1. Order of port buffers update
```yml
update_sequence: <type>
```
##### ```DEFAULT``` - rund-robin update order
##### ```[P1, P2, ...]``` - fixed update order of ports


#### 2. Number of flit slots per buffer
```yml
buffer_depth: <depth>
```


#### 3. Minimum length in flits of generated packets
```yml
min_packet_size: <size>
```


#### 4. Maximum length in flits of generated packets
```yml
max_packet_size: <size>
```


#### 5. Controls injection rate in number of flits instead of packets
```yml
flit_injection_rate: <true/false>
```


#### 6. Controls scaling of generated packets / flits with number of nodes
```yml
scale_with_nodes: <true/false>
```


#### 7. Probability of packet / flit generation for node per every simulation iteration
```yml
packet_injection_rate: <rate>
```