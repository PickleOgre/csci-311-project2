// Airport Simulation Project
// Author: Joe Lawrence
// Class: Algorithms and Data Structures
// Semester: Fall 2025

#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;
using std::vector;

/* # Objects */
struct Aircraft {
  unsigned int sim_time; // Time at which this aircraft enters the sim
  int id;                // A unique aircraft identifier
  string heading;        // A string, "arriving" or "departing"
  unsigned int priority; // Lower priority values are handled first.

  // Constructor
  Aircraft(unsigned int sim_time_val = 0, int id_val = 0,
           string heading_val = "", unsigned int priority_val = 0)
      : sim_time(sim_time_val), id(id_val), heading(heading_val),
        priority(priority_val) {}

  // Get all attributes as string
  string stats();

  // Returns true if this aircraft comes before a
  bool comesBefore(const Aircraft &a);
};

class PriorityQueue {
private:
  // Heap-based priority queue
  vector<Aircraft> queue;

  // Heap functions
  size_t parent(size_t i);
  size_t leftChild(size_t i);
  size_t rightChild(size_t i);
  void heapifyDown(size_t i);
  void heapifyUp(size_t i);

public:
  // Queue functions
  bool empty();
  size_t size();
  void push(const Aircraft &a);
  std::optional<Aircraft> pop();
  std::optional<Aircraft> peek();

  // Constructor
  PriorityQueue(vector<Aircraft> queue_val = {}) {
    if (!queue_val.empty()) {
      for (Aircraft a : queue_val) {
        push(a);
      }
    }
  }
};
/* # Aircraft functions */
string Aircraft::stats() {
  std::stringstream ss;
  ss << sim_time << " " << id << " " << heading << " " << priority;
  return ss.str();
}

bool Aircraft::comesBefore(const Aircraft &a) {
  // Compare by priority
  if (priority < a.priority)
    return true;
  else if (priority > a.priority)
    return false;

  // Compare by heading
  if (heading == "departing" && a.heading == "arriving")
    return true;
  if (heading == "arriving" && a.heading == "departing")
    return false;

  // Compare by ID
  if (id < a.id)
    return true;
  else if (id > a.id)
    return false;

  // if all fields are equal
  return false;
}

/* # Heap functions */
size_t PriorityQueue::parent(size_t i) { return (i - 1) / 2; }
size_t PriorityQueue::leftChild(size_t i) { return 2 * i + 1; }
size_t PriorityQueue::rightChild(size_t i) { return 2 * i + 2; }

void PriorityQueue::heapifyDown(size_t i) {
  size_t first = i;
  size_t l = leftChild(i);
  size_t r = rightChild(i);

  // If left is higher priority than root
  if (l < size() && queue[l].comesBefore(queue[first]))
    first = l;

  // If right is higher priority than the highest so far
  if (r < size() && queue[r].comesBefore(queue[first]))
    first = r;

  // If first is not root
  if (first != i) {
    std::swap(queue[i], queue[first]);

    // Recursively downward-heapify the altered subtree
    heapifyDown(first);
  }
}

void PriorityQueue::heapifyUp(size_t i) {
  if (i == 0)
    return;

  size_t p = parent(i);
  // Recursively upward-heapify until i is in the correct place
  if (queue[i].comesBefore(queue[p])) {
    std::swap(queue[i], queue[p]);
    heapifyUp(p);
  }
}

/* # Queue functions */
bool PriorityQueue::empty() { return queue.empty(); }

size_t PriorityQueue::size() { return queue.size(); }

// push() adds an element to the priority queue
void PriorityQueue::push(const Aircraft &a) {
  // Insert at the end of the heap
  queue.push_back(a);

  // Heapify upward from the last element
  heapifyUp(size() - 1);
}

// pop() removes and returns the highest priority element in the queue
std::optional<Aircraft> PriorityQueue::pop() {
  if (empty()) {
    return std::nullopt;
  }
  // Get the aircraft from root
  Aircraft root = queue[0];

  // Replace the element at root with the last element
  queue[0] = queue[size() - 1];

  // Remove the last element
  queue.pop_back();

  // Heapify down from root
  if (!empty())
    heapifyDown(0);

  // Return root, the highest-priority element
  return root;
}

// returns the highest-priority element in the queue without deleting int
std::optional<Aircraft> PriorityQueue::peek() {
  if (empty()) {
    return std::nullopt;
  }
  return queue[0];
}

/* # Simulation Functions */
int main() {
  vector<Aircraft> all_aircraft;
  PriorityQueue pq_departures;
  PriorityQueue pq_arrivals;
  unsigned int aircraft_count;

  // Get the number of planes to be simulated from stdin
  std::cin >> aircraft_count;

  // cin the details of each plane and store them in an array all_aircraft
  for (unsigned int i = 0; i < aircraft_count; i++) {
    unsigned int sim_time_val = 0;
    int id_val = 0;
    string heading_val = "";
    unsigned int priority_val = 0;

    std::cin >> sim_time_val >> id_val >> heading_val >> priority_val;
    Aircraft new_aircraft(sim_time_val, id_val, heading_val, priority_val);
    all_aircraft.push_back(new_aircraft);
  }

  // Begin simulation
  unsigned int t = 0; // Start at time step 0
  std::optional<Aircraft> runway_A;
  std::optional<Aircraft> runway_B;
  while (!all_aircraft.empty() || !pq_arrivals.empty() ||
         !pq_departures.empty()) {
    // Add aircraft with sim_time t to a vector entering_aircraft
    std::vector<Aircraft> entering_aircraft;
    if (!all_aircraft.empty()) {
      size_t i = 0;
      while (i < all_aircraft.size()) {
        if (all_aircraft[i].sim_time == t) {
          entering_aircraft.push_back(all_aircraft[i]);
          all_aircraft.erase(all_aircraft.begin() + i);
          continue;
        }
        i++;
      }
    }

    // If nothing needs to be done, increment time and do nothing.
    if (pq_departures.empty() && pq_arrivals.empty() &&
        entering_aircraft.empty()) {
      t++;
      continue;
    }

    // Print time
    std::cout << "Time step " << t << std::endl;

    // Push aircrafts from entering_aircraft to queues
    std::cout << "\tEntering simulation" << std::endl;
    size_t i = 0;
    while (i < entering_aircraft.size()) {
      std::cout << "\t\t" << entering_aircraft[i].stats() << std::endl;

      // Allocate to a queue
      if (entering_aircraft[i].heading == "departing") {
        pq_departures.push(entering_aircraft[i]);
      } else {
        pq_arrivals.push(entering_aircraft[i]);
      }

      // Remove from all_aircraft
      entering_aircraft.erase(entering_aircraft.begin() + i);
    }

    // Clear runways
    if (runway_A)
      runway_A.reset();
    if (runway_B)
      runway_B.reset();

    // Allocate aircrafts to runways
    if (!pq_departures.empty() && !pq_arrivals.empty()) {
      // Allocate departure to A and arrival to B.
      runway_A = pq_departures.pop();
      runway_B = pq_arrivals.pop();
    } else if (!pq_departures.empty()) {
      // Allocate any departures to A and then to B.
      runway_A = pq_departures.pop();
      if (!pq_departures.empty())
        runway_B = pq_departures.pop();
    } else if (!pq_arrivals.empty()) {
      // Allocate any arrivals to B and then to A.
      runway_B = pq_arrivals.pop();
      if (!pq_arrivals.empty())
        runway_A = pq_arrivals.pop();
    }

    // Print runway information
    std::cout << "\tRunway A" << std::endl;
    if (runway_A)
      std::cout << "\t\t" << runway_A->stats() << std::endl;
    std::cout << "\tRunway B" << std::endl;
    if (runway_B)
      std::cout << "\t\t" << runway_B->stats() << std::endl;

    t++;
  }
}
