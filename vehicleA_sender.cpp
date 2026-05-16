#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <pthread.h>
#include <sched.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5555
#define WIFI_RANGE 100.0       // meters
#define WIFI_DELAY_MS 50       // ms
#define LORA_DELAY_SEC 2       // seconds
#define WIFI_LOSS 5            // %
#define LORA_LOSS 20           // %
#define EARTH_RADIUS 6371000.0
struct Vehicle {
   char id;
   double lat;
   double lon;
   double speed;
};
void set_realtime_priority(int priority) {
   pthread_t self = pthread_self();
   struct sched_param param;
   param.sched_priority = priority;
   if (pthread_setschedparam(self, SCHED_FIFO, &param) != 0) {
       perror("SCHED_FIFO set failed");
   } else {
       std::cout << " Sender running with SCHED_FIFO, priority "
                 << priority << std::endl;
   }
}
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
bool packetLost(int probability) {
   return (rand() % 100) < probability;
}
void moveVehicle(Vehicle &v) {
   v.lat += ((rand() % 10) - 5) * 0.00001;
   v.lon += ((rand() % 10) - 5) * 0.00001;
   v.speed = (rand() % 80) + 20;
}
int main() {
   srand(time(nullptr));
   set_realtime_priority(30);
   int sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
       perror("Socket creation failed");
       return 1;
   }
   sockaddr_in recvAddr{};
   recvAddr.sin_family = AF_INET;
   recvAddr.sin_port = htons(PORT);
   recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   Vehicle vehicles[] = {
       {'A', 12.9716, 77.5946, 40},
       {'B', 12.9719, 77.5949, 45},
       {'C', 12.9722, 77.5952, 35}
   };
   std::cout << " Vehicle A Sender started (Real-Time)\n";
   while (true) {
       for (auto &v : vehicles) {
           moveVehicle(v);
           double dist = distance(
               vehicles[0].lat, vehicles[0].lon,
               v.lat, v.lon
           );
           const char *protocol;
           int loss;
           if (dist < WIFI_RANGE) {
               protocol = "WIFI";
               usleep(WIFI_DELAY_MS * 1000);
               loss = WIFI_LOSS;
           } else {
               protocol = "LORA";
               sleep(LORA_DELAY_SEC);
               loss = LORA_LOSS;
           }
           if (packetLost(loss)) {
               std::cout << " Packet lost from Vehicle "
                         << v.id << " (" << protocol << ")\n";
               continue;
           }
           char msg[256];
           snprintf(msg, sizeof(msg),
                    "%c,%.6f,%.6f,%.2f,%s",
                    v.id, v.lat, v.lon, v.speed, protocol);
           sendto(sock, msg, strlen(msg), 0,
                  (sockaddr*)&recvAddr, sizeof(recvAddr));
           std::cout << " Sent [" << protocol
                     << "] from Vehicle " << v.id << std::endl;
       }
       sleep(1);
   }
}

