#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <pthread.h>
#include <sched.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5555
#define COLLISION_DISTANCE 30.0
#define EARTH_RADIUS 6371000.0
/* ---------------- REAL-TIME SETUP ---------------- */
void set_realtime_priority(int priority) {
   pthread_t self = pthread_self();
   struct sched_param param;
   param.sched_priority = priority;
   if (pthread_setschedparam(self, SCHED_FIFO, &param) != 0) {
       perror("SCHED_FIFO set failed");
   } else {
       std::cout << " Receiver running with SCHED_FIFO, priority "
                 << priority << std::endl;
   }
}
/* ---------------- UTILITY ---------------- */
double toRad(double deg) {
   return deg * M_PI / 180.0;
}
double distance(double lat1, double lon1, double lat2, double lon2) {
   double dLat = toRad(lat2 - lat1);
   double dLon = toRad(lon2 - lon1);
   lat1 = toRad(lat1);
   lat2 = toRad(lat2);
   double a = sin(dLat/2)*sin(dLat/2) +
              cos(lat1)*cos(lat2)*sin(dLon/2)*sin(dLon/2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   return EARTH_RADIUS * c;
}
/* ---------------- MAIN ---------------- */
int main() {
   /* High priority: safety-critical ECU */
   set_realtime_priority(50);
   int sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
       perror("Socket creation failed");
       return 1;
   }
   sockaddr_in addr{};
   addr.sin_family = AF_INET;
   addr.sin_port = htons(PORT);
   addr.sin_addr.s_addr = INADDR_ANY;
   if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
       perror("Bind failed");
       return 1;
   }
   /* Fixed position of receiver vehicle */
   double myLat = 12.9718;
   double myLon = 77.5948;
   char buffer[256];
   std::cout << " Vehicle B Receiver started (Real-Time)\n";
   while (true) {
       recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);
       char id;
       double lat, lon, speed;
       char protocol[6];
       sscanf(buffer, "%c,%lf,%lf,%lf,%s",
              &id, &lat, &lon, &speed, protocol);
       double dist = distance(myLat, myLon, lat, lon);
       std::cout << " From Vehicle " << id
                 << " | Protocol: " << protocol
                 << " | Distance: " << dist << " m\n";
       if (dist < COLLISION_DISTANCE) {
           std::cout << " COLLISION WARNING with Vehicle "
                     << id << std::endl;
       }
   }
}

