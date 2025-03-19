# Producer-Consumer Problem

## Overview
This project implements the **Bounded-Buffer Producer-Consumer Problem** using **System V IPC mechanisms** such as **shared memory** and **semaphores**. It simulates multiple producers that generate commodity prices and a single consumer that displays a dashboard of the latest prices.

## Files
- **producer.c**: Implements the producer process that generates random commodity prices and writes them to shared memory.
- **consumer.c**: Implements the consumer process that reads from shared memory and displays the updated commodity prices.
- **Makefile**: Automates the compilation of `producer.c` and `consumer.c`.

## Compilation
To compile the project, use the provided **Makefile**:
```sh
make
```
This will generate two executable files:
- `producer`
- `consumer`

## Running the Program
### Running a Producer
Use the following command to start a producer:
```sh
./producer <commodity_name> <mean_price> <std_dev> <sleep_time_ms> <buffer_size>
```
**Example:**
```sh
./producer GOLD 1800 50 200 40
```
This runs a producer for GOLD with a mean price of 1800, standard deviation of 50, and generates a new price every 200ms.

### Running the Consumer
Use the following command to start the consumer:
```sh
./consumer <buffer_size>
```
**Example:**
```sh
./consumer 40
```
This runs the consumer process, which reads the latest prices from the shared memory.

## Features
- **Multiple Producers:** Up to 20 producers can run simultaneously, each updating a different commodity price.
- **Single Consumer:** Reads and displays prices along with an **average of the last 5 values**.
- **Synchronization:** Implemented using **semaphores** to prevent race conditions.
- **Shared Memory:** Used to store and retrieve commodity prices.
- **Logging:** Producers log their actions to `stderr`.

## Example Consumer Output
```
+-----------------+--------+-----------+
| Commodity      | Price  | AvgPrice  |
+-----------------+--------+-----------+
| GOLD           | 1820.50↑ | 1815.25↑ |
| SILVER         | 22.36↓  | 22.80↓   |
| COPPER         | 0.00   | 0.00     |
+-----------------+--------+-----------+
```

## Dependencies
This project requires **Linux** and a **C compiler (GCC)**.

## Cleanup
To remove compiled binaries, use:
```sh
make clean
```

## Notes
- Ensure that **System V IPC support** is enabled in your Linux system.
- The buffer size should be the same for all producers and the consumer.

