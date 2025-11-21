// Airport Simulation Project
// Author: Joe Lawrence
// Class: Algorithms and Data Structures
// Semester: Fall 2025

#include <iostream>
#include <memory>
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
  bool comesBefore(Aircraft a);
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
  void push(Aircraft a);
  Aircraft pop();
  Aircraft peek();

  // Constructor
  PriorityQueue(vector<Aircraft> queue_val = {}) {
    if (queue.size() > 0) { // if initialized with an existing array, construct
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

bool Aircraft::comesBefore(Aircraft a) {
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
void PriorityQueue::push(Aircraft a) {
  // Insert at the end of the heap
  queue.push_back(a);

  // Heapify upward from the last element
  heapifyUp(size() - 1);
}

// pop() removes and returns the highest priority element in the queue
Aircraft PriorityQueue::pop() {
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
Aircraft PriorityQueue::peek() { return queue[0]; }

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

  // Debug
  // for (Aircraft a : all_aircraft)
  //  std::cout << "All Aircraft: " << a.stats() << std::endl;

  // Begin simulation
  unsigned int t = 0; // Start at time step 0
  std::optional<Aircraft> runway_A;
  std::optional<Aircraft> runway_B;
  while (!all_aircraft.empty() || !pq_arrivals.empty() ||
         !pq_departures.empty()) {
    // if there is nothing to be done for this time step, continue
    if (pq_departures.empty() && pq_arrivals.empty()) {
      bool new_aircraft_entering = false;
      for (Aircraft a : all_aircraft) {
        if (a.sim_time == t) {
          new_aircraft_entering = true;
          break;
        }
      }
      if (!new_aircraft_entering) {
        t++;
        continue;
      }
    }

    std::cout << "Time step " << t << std::endl;

    // Check for Aircraft in the vector with sim_time == t and push to queue
    std::cout << "\tEntering simulation" << std::endl;
    size_t a = 0;
    while (a < all_aircraft.size()) {
      if (all_aircraft[a].sim_time == t) {
        std::cout << "\t\t" << all_aircraft[a].stats() << std::endl;

        // Allocate to a queue
        if (all_aircraft[a].heading == "departing") {
          pq_departures.push(all_aircraft[a]);
        } else {
          pq_arrivals.push(all_aircraft[a]);
        }

        // Remove from all_aircraft
        all_aircraft.erase(all_aircraft.begin() + a);
        continue;
      }
      a++;
    }

    // Debug
    // std::cout << "\tQueue: " << std::endl;
    // for (Aircraft a : pq.queue) {
    //   std::cout << "\t\t" << a.stats() << std::endl;
    // }

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

    /*
        // Clear runways and put new aircraft in them
        for (size_t r = 0; r < NUM_RUNWAYS; r++) {
          if (pq.size() > 0) {
            std::cout << "\tRunway " << (char)('A' + (r % 26)) << std::endl;
            runways[r] = pq.pop();
            std::cout << "\t\t" << runways[r].stats() << std::endl;
          }
        }
    */
    // Increment time step
    t++;
  }
  // Debug:
  // std::cout << "Done!" << std::endl;
}
