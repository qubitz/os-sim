Operating System Simulator
==========================

Overview
--------
Small operating system simulator

Features:
* Mutex locks
* Semaphores
* File I/O
* Error handling
* Process scheduling
* Resource managing

![Screenshot](https://github.com/qubytes/os-sim/blob/actual/pics/screenshot.png)

How to Run
----------
* Linux
  1. **Install** dependancies
  ```bash
  sudo apt-get install libboost-all-dev
  ```
  2. **Build**
  ```bash
  make
  ```
  3. **Run**
  ```bash
  cd bin
  ./os_sim config_1.conf
  ```
  Command line argument 1 takes in config files located in ```bin```. Feel free to try those out!
  
* Mac
  1. **Install** dependancies
  ```bash
  brew install boost
  ```
  2. **Build**
  ```bash
  make
  ```
  3. **Run**
  ```bash
  cd bin
  ./os_sim config_1.conf
  ```
  Command line argument 1 takes in config files located in ```bin```. Feel free to try those out!
