# Smart V2V Communication System using QNX RTOS, QEMU, and Virtual Ethernet

## Overview

This project demonstrates a **Vehicle-to-Vehicle (V2V) Communication System** simulated in a **QNX Real-Time Operating System (RTOS)** environment using:

* QNX Momentics IDE
* QEMU Virtual Machine
* Virtual Ethernet (TAP Interface)
* UDP Socket Communication
* Real-Time Scheduling using `SCHED_FIFO`

The system simulates multiple vehicles exchanging location and speed information in real time.
Based on the distance between vehicles, the communication protocol dynamically switches between:

* **Wi-Fi** → Low latency, short range
* **LoRa** → Long range, higher latency

The project also includes:

* Packet loss simulation
* Collision warning system
* Real-time task prioritization
* Dynamic vehicle movement simulation

---

# System Architecture

## Components

| Component            | Purpose                                          |
| -------------------- | ------------------------------------------------ |
| Sender Node          | Simulates transmitting vehicle data              |
| Receiver Node        | Receives vehicle data and detects collision risk |
| UDP Socket           | Communication mechanism                          |
| QNX RTOS             | Real-time operating system                       |
| QEMU                 | Virtual target execution                         |
| TAP Virtual Ethernet | Network bridge between host and QNX target       |

---

# Technologies Used

* **C++**
* **QNX RTOS**
* **QNX Momentics IDE**
* **QEMU**
* **UDP Socket Programming**
* **POSIX Threads**
* **SCHED_FIFO Real-Time Scheduling**
* **Virtual Ethernet (TAP Adapter)**

---

# Project Workflow

## Sender Side Workflow

1. Vehicles are initialized with:

   * Vehicle ID
   * Latitude
   * Longitude
   * Speed

2. Vehicle position changes dynamically using random movement simulation.

3. Distance between vehicles is calculated using the Haversine Formula.

4. Based on distance:

   * If distance < 100m → Wi-Fi communication
   * Else → LoRa communication

5. Packet loss probability is simulated:

   * Wi-Fi → 5%
   * LoRa → 20%

6. Vehicle data is sent using UDP sockets.

7. Sender runs with real-time priority using:

   ```cpp
   SCHED_FIFO
   ```

---

## Receiver Side Workflow

1. Receiver creates and binds a UDP socket.

2. Continuously listens for incoming vehicle packets.

3. Extracts:

   * Vehicle ID
   * Latitude
   * Longitude
   * Speed
   * Communication protocol

4. Calculates distance between sender and receiver vehicle.

5. If distance becomes less than:

   ```cpp
   30 meters
   ```

   a collision warning is generated.

6. Receiver also runs using real-time scheduling with higher priority.

---

# Real-Time Scheduling

The project uses:

```cpp
pthread_setschedparam()
```

with:

```cpp
SCHED_FIFO
```

to simulate real-time ECU behavior.

## Priorities

| Module   | Priority |
| -------- | -------- |
| Receiver | 50       |
| Sender   | 30       |

The receiver is assigned higher priority because collision detection is safety critical.

---

# Communication Protocol Switching

## Wi-Fi Mode

Activated when:

```cpp
distance < 100 meters
```

### Characteristics

* Low latency
* Faster transmission
* Lower packet loss
* Simulated delay:

  ```cpp
  50 ms
  ```

---

## LoRa Mode

Activated when:

```cpp
distance >= 100 meters
```

### Characteristics

* Long-range communication
* Higher latency
* Higher packet loss
* Simulated delay:

  ```cpp
  2 seconds
  ```

---

# Packet Loss Simulation

The project simulates real-world communication instability.

| Protocol | Packet Loss |
| -------- | ----------- |
| Wi-Fi    | 5%          |
| LoRa     | 20%         |

Implemented using:

```cpp
rand() % 100
```

---

# Distance Calculation

The project uses the **Haversine Formula** to calculate geographical distance between vehicles.

d = 2R \arctan\left(\sqrt{\frac{a}{1-a}}\right)

Where:

* `R` → Earth radius
* `a` → Haversine intermediate value
* `d` → Distance between vehicles

This provides accurate GPS-based distance estimation.

---

# Collision Detection

If:

```cpp
distance < 30 meters
```

the receiver prints:

```text
COLLISION WARNING
```

This simulates emergency alert functionality in intelligent transportation systems.

---

# QNX Virtual Target Setup

## Requirements

Install:

* QNX SDP
* QNX Momentics IDE
* QEMU
* TAP Network Adapter

---

# QEMU Virtual Machine Configuration

The project uses the following `.bat` file to launch the QNX virtual target:

```bat
qemu-system-x86_64 ^
  -smp 2 ^
  --cpu max ^
  -m 1G ^
  -drive file=disk-qemu.vmdk,if=ide,id=drv0 ^
  -nographic ^
  -kernel ifs.bin ^
  -serial mon:stdio ^
  -no-reboot ^
  -netdev tap,id=net0,ifname=QEMU_TAP,script=no,downscript=no ^
  -device virtio-net-pci,netdev=net0,mac=52:54:00:cb:95:f2
```

---

# Explanation of QEMU Parameters

| Parameter           | Description                  |
| ------------------- | ---------------------------- |
| `-smp 2`            | Allocates 2 CPU cores        |
| `--cpu max`         | Uses maximum CPU feature set |
| `-m 1G`             | Allocates 1 GB RAM           |
| `-drive`            | Loads virtual disk           |
| `-kernel ifs.bin`   | Boots QNX image              |
| `-nographic`        | Runs in terminal mode        |
| `-serial mon:stdio` | Serial output to console     |
| `-netdev tap`       | Connects TAP virtual network |
| `virtio-net-pci`    | Virtual high-speed NIC       |

---

# Virtual Ethernet Setup

The TAP interface enables communication between:

* Host Machine
* QNX Virtual Target

## TAP Adapter Name

```text
QEMU_TAP
```

The virtual Ethernet connection allows UDP packet exchange between sender and receiver applications.

---

# Building the Project in QNX Momentics

## Steps

### 1. Create C++ Project

In QNX Momentics:

```text
File → New → QNX C/C++ Project
```

---

### 2. Add Source Files

Add:

* `sender.cpp`
* `receiver.cpp`

---

### 3. Build Project

Click:

```text
Build Project
```

Executable binaries will be generated.

---

# Running the Project

## Step 1 — Start QEMU

Run the `.bat` file:

```bat
start_qemu.bat
```

---

## Step 2 — Launch Receiver

Inside QNX target:

```bash
./receiver
```

---

## Step 3 — Launch Sender

In another terminal:

```bash
./sender
```

---

# Expected Output

## Sender Output

```text
Sender running with SCHED_FIFO, priority 30
Sent [WIFI] from Vehicle A
Sent [LORA] from Vehicle B
```

---

## Receiver Output

```text
Receiver running with SCHED_FIFO, priority 50
From Vehicle A | Protocol: WIFI | Distance: 24 m
COLLISION WARNING with Vehicle A
```

---

# Features Implemented

* Real-Time Scheduling
* UDP Communication
* Dynamic Protocol Switching
* Packet Loss Simulation
* Collision Warning System
* GPS Distance Calculation
* Virtual Ethernet Networking
* QNX RTOS Deployment
* QEMU Virtual Target Simulation

---

# Future Enhancements

* Integration with real GPS modules
* LoRa hardware deployment
* Multi-hop V2V communication
* GUI dashboard for vehicle monitoring
* AI-based collision prediction
* DSRC protocol integration
* Secure encrypted communication

---

# File Structure

```text
├── sender.cpp
├── receiver.cpp
├── start_qemu.bat
├── ifs.bin
├── disk-qemu.vmdk
└── README.md
```

---

# Applications

* Intelligent Transportation Systems
* Autonomous Vehicles
* Smart Traffic Management
* Emergency Vehicle Communication
* Accident Prevention Systems

