# Thread Synchronization in Student Bus Transport 

## 📜 Description  
This project simulates a student transportation system using threads and POSIX semaphores for synchronization.  

## 🚌 Functionality  
- Each student is a thread that moves between bus stops and the university.  
- The bus has a limited capacity (N), allowing up to (N/4) students per department.  
- Students wait at bus stops and board the bus following FIFO order, if space allows.  
- The university has unlimited capacity, and students stay there for a random study period (5-15 sec).  
- The simulation ends when all students return home.  

## ⚙️ Implementation Details  
- Written in **C** using **POSIX threads** and **semaphores**.  
- Uses **random(3)** for study time assignment.   
- Prints messages to indicate student actions (arrival, boarding, studying, returning).  
- Prevents starvation and ensures fair student transportation.  

## 🏗️ Compilation & Execution  
  - make
  - ./threads

##Input
  - a. give number of students and 
  - b. number of Bus capasity 
